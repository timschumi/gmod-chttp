#include <curl/curl.h>
#include <string.h>
#include <unistd.h>

#include "HTTPRequest.h"
#include "Logger.h"
#include "RequestWorker.h"

// Taken from the curl configure script
static const char *capaths[] = {
	"/etc/ssl/certs/ca-certificates.crt",       // Debian
	"/etc/pki/tls/certs/ca-bundle.crt",         // Redhat and Mandriva
	"/usr/share/ssl/certs/ca-bundle.crt",       // Old Redhat
	"/usr/local/share/certs/ca-root-nss.crt",   // FreeBSD
	"/etc/ssl/cert.pem",                        // OpenBSD, FreeBSD
};

static const char *cabundle = nullptr;
static const char *src_if = nullptr;

HTTPRequest::HTTPRequest() {
	curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);
	this->headers["User-Agent"] = (std::string) "curl/" + info->version + " gmod-chttp/1.5.1";
}

std::string HTTPRequest::buildQueryString() {
	std::string params;

	for (auto const &e : this->parameters) {
		if (!params.empty())
			params += "&";

		char *key = curl_easy_escape(nullptr, e.first.c_str(), e.first.length());
		char *val = curl_easy_escape(nullptr, e.second.c_str(), e.second.length());

		params += key;
		params += "=";
		params += val;

		curl_free(key);
		curl_free(val);
	}

	return params;
}

std::string HTTPRequest::buildURL() {
	std::string fullurl = this->url;

	// Do we need a query string?
	if (!this->method.isLikePost() && !this->parameters.empty()) {
		fullurl += "?" + this->buildQueryString();
	}

	return fullurl;
}

void curlAddHeaders(CURL *curl, HTTPRequest *request) {
	struct curl_slist *headers = nullptr;

	// Add all the headers from the request struct
	for (auto const &e : request->headers)
		headers = curl_slist_append(headers, (e.first + ": " + e.second).c_str());

	// Add the header list to the curl struct
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

void curlSetMethod(CURL *curl, HTTPMethod method) {
	if (method.isLikePost())
		curl_easy_setopt(curl, CURLOPT_POST, 1L);

	// METHOD_GET and METHOD_POST are not listed here,
	// since they don't require any specific setup
	switch (method) {
	case HTTPMethod::M_HEAD:
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		break;
	case HTTPMethod::M_PUT:
	case HTTPMethod::M_DELETE:
	case HTTPMethod::M_PATCH:
	case HTTPMethod::M_OPTIONS:
	case HTTPMethod::M_INVALID:
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.toString().c_str());
		break;
	default:
		break;
	}
}

#ifdef __linux__

const char *findCABundle() {
	if (auto capath = getenv("CHTTP_CAINFO")) {
		Logger::msg("Forcing CAINFO to '%s'", capath);
		return capath;
	}

	for (auto &capath : capaths) {
		if (access(capath, R_OK) == 0) {
			Logger::msg("Found accessible CAINFO in '%s'", capath);
			return capath;
		}
	}

	Logger::warn("Found no suitable CAINFO!");
	return nullptr;
}

#endif

// Write callback for appending to an std::string
size_t curl_string_append(char *contents, size_t size, size_t nmemb, std::string *userp) {
	userp->append(contents, size * nmemb);
	return size * nmemb;
}

// Write callback for appending to an std::map (will split header at the first colon)
size_t curl_headermap_append(char *contents, size_t size, size_t nmemb, std::map<std::string, std::string> *userp) {
	std::string header(contents, size * nmemb);

	std::size_t found = header.find_first_of(':');

	if (found != std::string::npos) {
		(*userp)[header.substr(0, found)] = header.substr(found + 2, header.length() - found - 4);
	}

	return size * nmemb;
}

bool strstart(const char *string, const char *prefix) {
	return strncmp(string, prefix, strlen(prefix)) == 0;
}

bool HTTPRequest::run() {
	CURL *curl = curl_easy_init();

	if (!curl) {
		RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(this->failed, "Failed to init curl struct!"));
		return false;
	}

	CURLcode cres;
	bool ret = true;
	auto response = std::make_shared<SuccessCallbackTask>(this->success);
	std::string postbody;
	const char *redirect;

#ifdef __linux__
	// Find the CA bundle if not already cached
	if (!cabundle)
		cabundle = findCABundle();

	// Set the CA path
	curl_easy_setopt(curl, CURLOPT_CAINFO, cabundle);
#endif

	// Query the source interface from the environment variables
	if (!src_if && (src_if = getenv("CHTTP_INTERFACE"))) {
		Logger::msg("Forcing INTERFACE to '%s'", src_if);
	}

	// Override the source interface if set
	if (src_if) {
		curl_easy_setopt(curl, CURLOPT_INTERFACE, src_if);
	}

	if (this->timeout != 0) {
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
	}

	curlSetMethod(curl, this->method);

	if (this->method.isLikePost()) {
		// Do we have a request body?
		if (!this->body.empty()) {
			postbody = this->body;
			this->headers["Content-Type"] = type;
		} else {
			postbody = this->buildQueryString();
		}

		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postbody.size());
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, postbody.c_str());
	}

	// Set up saving the response body
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response->body);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_string_append);

	// Set up saving the headers
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response->headers);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_headermap_append);

	curlAddHeaders(curl, this);

	std::string built_url = this->buildURL();
	curl_easy_setopt(curl, CURLOPT_URL, built_url.c_str());
	Logger::devmsg("Sending a request to '%s'...", built_url.c_str());

resend:
	char *effective_url;
	curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);

	// Disallow requests with non-HTTP protocols
	if (effective_url && !strstart(effective_url, "https://") && !strstart(effective_url, "http://")) {
		RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(this->failed, "Non-HTTP protocol"));
		ret = false;
		goto cleanup;
	}

	cres = curl_easy_perform(curl);

	if (cres != CURLE_OK) {
		RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(this->failed, curl_easy_strerror(cres)));
		ret = false;
		goto cleanup;
	}

	curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirect);
	if (redirect) {
		// Clear out saved headers and body
		response->headers.clear();
		response->body.clear();

		// Set the new URL and clear the temp variable
		curl_easy_setopt(curl, CURLOPT_URL, redirect);
		Logger::devmsg("Redirecting to '%s'...", redirect);
		redirect = "";

		goto resend;
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->code);

	RequestWorker::the().tasks().push(response);

cleanup:
	curl_easy_cleanup(curl);

	return ret;
}
