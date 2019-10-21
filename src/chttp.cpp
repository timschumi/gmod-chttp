#include <string>
#include <curl/curl.h>
#include "GarrysMod/Lua/Interface.h"
#include "chttp.h"
#include "http.h"
#include "method.h"
#include "lua.h"

using namespace GarrysMod;

std::string buildUserAgent() {
	std::string user = "";
	curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);

	user += "User-Agent:";

	user += " curl/";
	user += info->version;

	user += " gmod-chttp/";
	user += CHTTP_VERSION;

	return user;
}

void dumpRequest(Lua::ILuaBase *LUA, HTTPRequest request) {
	LOG("Dumping request:");
	LOG("url: " + request.url);
	LOG("method: " + methodToString(request.method));
	LOG("headers: [" + std::to_string(request.headers.size()) + "]");
	for (auto const& e : request.headers) {
		LOG("  " + e.first + ": " + e.second);
	}
	LOG("parameters: [" + std::to_string(request.parameters.size()) + "]");
	for (auto const& e : request.parameters) {
		LOG("  " + e.first + ": " + e.second);
	}
	LOG("body: " + request.body);
	LOG("type: " + request.type);
}

void dumpResponse(Lua::ILuaBase *LUA, HTTPResponse response) {
	LOG("Dumping response:");
	LOG("code: " + std::to_string(response.code));
	LOG("headers: [" + std::to_string(response.headers.size()) + "]");
	for (auto const& e : response.headers) {
		LOG("  " + e.first + ": " + e.second);
	}
	LOG("body: " + response.body);
}

void runFailedHandler(Lua::ILuaBase *LUA, HTTPRequest request, std::string reason) {
	// The request doesn't have a failure handler attached,
	// so just print the error in the log.
	if (!request.failed) {
		printMessage(LUA, "[failed] reason: " + reason);
		return;
	}

	// Push fail handler to stack and free our ref
	LUA->ReferencePush(request.failed);
	LUA->ReferenceFree(request.failed);

	// Push the argument
	LUA->PushString(reason.c_str());

	// Call the fail handler with one argument
	LUA->Call(1, 0);
}

void runSuccessHandler(Lua::ILuaBase *LUA, HTTPRequest request, HTTPResponse response) {
	// If we don't have a success handler defined, just dump it to the console
	if (!request.success) {
		LOG("No success handler defined!");
#ifdef DEBUG_BUILD
		dumpResponse(LUA, response);
#endif
		return;
	}

	// Push success handler to stack and free our ref
	LUA->ReferencePush(request.success);
	LUA->ReferenceFree(request.success);

	// Push the arguments
	LUA->PushNumber(response.code);
	LUA->PushString(response.body.c_str());
	mapToLuaTable(LUA, response.headers);

	// Call the success handler with three arguments
	LUA->Call(3, 0);
}

void curlAddHeaders(CURL *curl, HTTPRequest request) {
	struct curl_slist *headers = NULL;

	// Check if we have to add the default User-Agent
	if (request.headers.count("User-Agent") == 0)
		headers = curl_slist_append(headers, buildUserAgent().c_str());

	// Add the Content-Type header if not already set
	if (request.headers.count("Content-Type") == 0)
		headers = curl_slist_append(headers, ("Content-Type: " + request.type).c_str());

	// Add all the headers from the request struct
	for (auto const& e : request.headers)
		headers = curl_slist_append(headers, (e.first + ": " + e.second).c_str());

	// Add the header list to the curl struct
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

void curlSetMethod(CURL *curl, int method) {
	if (isLikePost(method))
		curl_easy_setopt(curl, CURLOPT_POST, 1L);

	// METHOD_GET and METHOD_POST are not listed here,
	// since they don't require any specific setup
	switch (method) {
	case METHOD_HEAD:
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		break;
	case METHOD_PUT:
	case METHOD_DELETE:
	case METHOD_PATCH:
	case METHOD_OPTIONS:
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, methodToString(method).c_str());
		break;
	}
}

// Write callback for appending to an std::string
size_t curl_string_append(char *contents, size_t size, size_t nmemb, std::string *userp) {
	userp->append(contents, size * nmemb);
	return size * nmemb;
}

// Write callback for appending to an std::map (will split header at the first colon)
size_t curl_headermap_append(char *contents, size_t size, size_t nmemb, std::map<std::string, std::string> *userp) {
	std::string header(contents, size * nmemb);

	std::size_t found = header.find_first_of(":");

	if (found != std::string::npos) {
		(*userp)[header.substr(0, found)] = header.substr(found + 2, header.length() - found - 4);
	}

	return size * nmemb;
}

bool processRequest(Lua::ILuaBase *LUA, HTTPRequest request) {
	CURL *curl;
	CURLcode cres;
	bool ret = true;
	HTTPResponse response = HTTPResponse();
	std::string postbody = "";
	char* redirect = "";

	curl = curl_easy_init();

	if (!curl) {
		runFailedHandler(LUA, request, "Failed to init curl struct!");
		ret = false;
		goto cleanup;
	}

	curlSetMethod(curl, request.method);

	if (isLikePost(request.method)) {
		// Do we have a request body?
		if (request.body.size() != 0) {
			postbody = request.body;
		} else {
			postbody = buildParameters(request);
		}

		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postbody.size());
		curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, postbody.c_str());
	}

	// Set up saving the response body
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_string_append);

	// Set up saving the headers
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, curl_headermap_append);

	curlAddHeaders(curl, request);

resend:
	curl_easy_setopt(curl, CURLOPT_URL, buildUrl(request).c_str());

	cres = curl_easy_perform(curl);

	if (cres != CURLE_OK) {
		runFailedHandler(LUA, request, curl_easy_strerror(cres));
		ret = false;
		goto cleanup;
	}

	curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &redirect);
	if (redirect) {
#ifdef DEBUG_BUILD
		LOG("Redirecting to: " + std::string(redirect));
#endif

		// Clear out saved headers and body
		response.headers.clear();
		response.body.clear();

		// Set the new URL and clear the temp variable
		request.url = redirect;
		redirect = "";

		goto resend;
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.code);

	runSuccessHandler(LUA, request, response);

cleanup:
	if (curl)
		curl_easy_cleanup(curl);

	return ret;
}

/*
 * See https://wiki.garrysmod.com/page/Global/HTTP for documentation.
 * The function takes a single table argument, based off the HTTPRequest structure.
 * It returns a boolean whether a request was sent or not.
 */
LUA_FUNCTION(CHTTP) {
	HTTPRequest request = HTTPRequest();
	bool ret;

	if (!LUA->IsType(1, Lua::Type::TABLE)) {
		LOG("No HTTPRequest table set.");
		ret = false;
		goto exit;
	}

	// Fetch failed handler
	LUA->GetField(1, "failed");
	if (LUA->IsType(-1, Lua::Type::FUNCTION)) {
		request.failed = LUA->ReferenceCreate();
	} else {
		LUA->Pop();
	}

	// Fetch method
	LUA->GetField(1, "method");
	if (LUA->IsType(-1, Lua::Type::STRING)) {
		request.method = methodFromString(LUA->GetString(-1));
	} else {
		request.method = METHOD_GET;
	}
	if (request.method == METHOD_NOSUPP) {
		runFailedHandler(LUA, request, "Unsupported request method: " + std::string(LUA->GetString(-1)));
		ret = false;
		goto exit;
	}
	LUA->Pop();

	// Fetch url
	LUA->GetField(1, "url");
	if (LUA->IsType(-1, Lua::Type::STRING)) {
		request.url = LUA->GetString(-1);
	} else {
		runFailedHandler(LUA, request, "invalid url");
		ret = false;
		goto exit;
	}
	LUA->Pop();

	// Fetch success handler
	LUA->GetField(1, "success");
	if (LUA->IsType(-1, Lua::Type::FUNCTION)) {
		request.success = LUA->ReferenceCreate();
	} else {
		LUA->Pop();
	}

	// Fetch headers
	LUA->GetField(1, "headers");
	if (LUA->IsType(-1, Lua::Type::TABLE)) {
		request.headers = mapFromLuaTable(LUA, -1);
	}
	LUA->Pop();

	// Fetch parameters
	LUA->GetField(1, "parameters");
	if (LUA->IsType(-1, Lua::Type::TABLE)) {
		request.parameters = mapFromLuaTable(LUA, -1);
	}
	LUA->Pop();

	// Fetch type
	LUA->GetField(1, "type");
	if (LUA->IsType(-1, Lua::Type::STRING)) {
		request.type = LUA->GetString(-1);
	} else {
		request.type = "text/plain; charset=utf-8";
	}
	LUA->Pop();

	// Fetch body
	LUA->GetField(1, "body");
	if (LUA->IsType(-1, Lua::Type::STRING)) {
		request.body = LUA->GetString(-1);
	}
	LUA->Pop();

#ifdef DEBUG_BUILD
	dumpRequest(LUA, request);
#endif

	ret = processRequest(LUA, request);

exit:
	LUA->PushBool(ret); // Push result to the stack
	return 1; // We are returning a single value
}

GMOD_MODULE_OPEN() {
#ifdef WINDOWS_BUILD
	if (curl_global_sslset(CURLSSLBACKEND_SCHANNEL, NULL, NULL) != CURLSSLSET_OK) {
		LOG("error: The WinSSL/schannel backend is not available!");
		return 1;
	}
#endif

	// Initialize curl
	curl_global_init(CURL_GLOBAL_ALL);

	// We are working on the global table today
	LUA->PushSpecial(Lua::SPECIAL_GLOB);

	// Push the function mapping (first is the key/function name,
	// second is the value/actual function)
	LUA->PushString("CHTTP");
	LUA->PushCFunction(CHTTP);

	// SetTable takes the item at the top of the stack (value) and
	// the second item from the top (key) and adds them to the table
	// at the stack offset mentioned in the parameter (again, -1 is the top)
	LUA->SetTable(-3);

	// Pop the global table from the stack again
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	// Cleanup curl
	curl_global_cleanup();

	return 0;
}