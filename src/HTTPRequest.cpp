#include <curl/curl.h>
#include <fstream>
#include <string.h>
#include <unistd.h>

#include "HTTPRequest.h"
#include "Logger.h"
#include "RequestWorker.h"

// Taken from the curl configure script
static char const* ca_paths[] = {
    "/etc/ssl/certs/ca-certificates.crt",     // Debian
    "/etc/pki/tls/certs/ca-bundle.crt",       // Redhat and Mandriva
    "/usr/share/ssl/certs/ca-bundle.crt",     // Old Redhat
    "/usr/local/share/certs/ca-root-nss.crt", // FreeBSD
    "/etc/ssl/cert.pem",                      // OpenBSD, FreeBSD
};

static char const* source_interface = nullptr;

HTTPRequest::HTTPRequest()
{
    curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
    this->headers["User-Agent"] = (std::string) "curl/" + info->version + " gmod-chttp/" CHTTP_VERSION;
}

std::string HTTPRequest::build_query_string()
{
    std::string params;

    for (auto const& e : this->parameters) {
        if (!params.empty())
            params += "&";

        char* key = curl_easy_escape(nullptr, e.first.c_str(), e.first.length());
        char* val = curl_easy_escape(nullptr, e.second.c_str(), e.second.length());

        params += key;
        params += "=";
        params += val;

        curl_free(key);
        curl_free(val);
    }

    return params;
}

std::string HTTPRequest::build_url()
{
    std::string full_url = this->url;

    // Do we need a query string?
    if (!this->method.is_like_post() && !this->parameters.empty()) {
        full_url += "?" + this->build_query_string();
    }

    return full_url;
}

// Write callback for appending to an std::string
size_t curl_string_append(char* contents, size_t size, size_t nmemb, std::string* userp)
{
    userp->append(contents, size * nmemb);
    return size * nmemb;
}

// Write callback for appending to an std::map (will split header at the first colon)
size_t curl_headermap_append(char* contents, size_t size, size_t nmemb, std::map<std::string, std::string>* userp)
{
    std::string header(contents, size * nmemb);

    std::size_t found = header.find_first_of(':');

    if (found != std::string::npos) {
        (*userp)[header.substr(0, found)] = header.substr(found + 2, header.length() - found - 4);
    }

    return size * nmemb;
}

bool strstart(char const* string, char const* prefix)
{
    return strncmp(string, prefix, strlen(prefix)) == 0;
}

bool HTTPRequest::run()
{
    CURL* curl = curl_easy_init();

    if (!curl) {
        RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(this->failed, "Failed to init curl struct!"));
        return false;
    }

    CURLcode curl_result;
    bool ret = true;
    auto response = std::make_shared<SuccessCallbackTask>(this->success);
    std::string post_body;
    char const* redirect;

#ifdef __linux__
    // Find the CA bundle if not already cached
    static char const* ca_bundle_path = [&]() -> char const* {
        if (ca_bundle_path)
            return ca_bundle_path;

        if (auto capath = getenv("CHTTP_CAINFO")) {
            Logger::msg("Forcing CAINFO to '%s'", capath);
            return capath;
        }

        for (auto& ca_path : ca_paths) {
            if (access(ca_path, R_OK) == 0) {
                Logger::msg("Found accessible CAINFO at '%s'", ca_path);
                return ca_path;
            }
        }

        Logger::warn("Found no suitable CAINFO!");
        return nullptr;
    }();

    // Set the CA path
    curl_easy_setopt(curl, CURLOPT_CAINFO, ca_bundle_path);
#endif

    // Query the source interface from the environment variables
    if (!source_interface && (source_interface = getenv("CHTTP_INTERFACE"))) {
        Logger::msg("Forcing INTERFACE to '%s'", source_interface);
    }

    // Override the source interface if set
    if (source_interface) {
        curl_easy_setopt(curl, CURLOPT_INTERFACE, source_interface);
    }

    if (this->timeout != 0) {
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    }

    // Set up the request method
    switch (this->method) {
    case HTTPMethod::Get:
        // Default, does not require any additional setup
        break;
    case HTTPMethod::Head:
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        break;
    case HTTPMethod::Post:
    case HTTPMethod::Put:
    case HTTPMethod::Delete:
    case HTTPMethod::Patch:
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        // Intentional fallthrough to set request method name
    case HTTPMethod::Options:
    case HTTPMethod::Invalid:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, this->method.to_string().c_str());
        break;
    default:
        Logger::warn("HTTP request method is neither valid nor INVALID: %d", this->method);
        break;
    }

    if (this->method.is_like_post()) {
        // Do we have a request body?
        if (!this->body.empty()) {
            post_body = this->body;
            this->headers["Content-Type"] = type;
        } else {
            post_body = this->build_query_string();
        }

        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_body.size());
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, post_body.c_str());
    }

    // Set up saving the response body
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response->body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_string_append);

    // Set up saving the headers
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response->headers);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_headermap_append);

#ifdef __linux__
    // Check on the random generator
    [&]() {
        static bool already_checked = false;

        if (already_checked)
            return;

        already_checked = true;

        std::ifstream avail_node("/proc/sys/kernel/random/entropy_avail", std::ifstream::in);
        std::ifstream wakeup_node("/proc/sys/kernel/random/read_wakeup_threshold", std::ifstream::in);

        if (!avail_node || !wakeup_node)
            return;

        int avail, wakeup;
        avail_node >> avail;
        wakeup_node >> wakeup;

        if (avail < wakeup) {
            Logger::warn("/dev/random is configured to wake up at %d bits of entropy, but only has %d bits available.", wakeup, avail);
            Logger::warn("This is likely to cause a block when trying to establish SSL connections.");
        }
    }();
#endif

    // Add headers to the mix
    {
        struct curl_slist* header_list = nullptr;

        // Add all the headers from the request struct
        for (auto const& e : this->headers)
            header_list = curl_slist_append(header_list, (e.first + ": " + e.second).c_str());

        // Add the header list to the curl struct
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }

    std::string built_url = this->build_url();
    curl_easy_setopt(curl, CURLOPT_URL, built_url.c_str());
    Logger::devmsg("Sending a request to '%s'...", built_url.c_str());

resend:
    char* effective_url;
    curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);

    // Disallow requests with non-HTTP protocols
    if (effective_url && !strstart(effective_url, "https://") && !strstart(effective_url, "http://")) {
        RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(this->failed, "Non-HTTP protocol"));
        ret = false;
        goto cleanup;
    }

    curl_result = curl_easy_perform(curl);

    if (curl_result != CURLE_OK) {
        RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(this->failed, curl_easy_strerror(curl_result)));
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
