#include <string>
#include <curl/curl.h>

#include "GarrysMod/Lua/Interface.h"

#include "chttp.h"
#include "lua.h"
#include "threading.h"

void curlAddHeaders(CURL *curl, HTTPRequest *request) {
	struct curl_slist *headers = nullptr;

	// Add all the headers from the request struct
	for (auto const& e : request->headers)
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
	case HTTPMethod::HEAD:
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		break;
	case HTTPMethod::PUT:
	case HTTPMethod::DELETE:
	case HTTPMethod::PATCH:
	case HTTPMethod::OPTIONS:
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.toString().c_str());
		break;
	default:
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

	std::size_t found = header.find_first_of(':');

	if (found != std::string::npos) {
		(*userp)[header.substr(0, found)] = header.substr(found + 2, header.length() - found - 4);
	}

	return size * nmemb;
}

bool processRequest(HTTPRequest *request) {
	CURL *curl;
	CURLcode cres;
	bool ret = true;
	HTTPResponse *response = new HTTPResponse();
	std::string postbody;
	const char* redirect;

	curl = curl_easy_init();

	if (!curl) {
		getResultQueue().push(new FailedQueueData(request->success, request->failed, "Failed to init curl struct!"));
		ret = false;
		goto cleanup;
	}

	curlSetMethod(curl, request->method);

	if (request->method.isLikePost()) {
		// Do we have a request body?
		if (!request->body.empty()) {
			postbody = request->body;
		} else {
			postbody = request->buildQueryString();
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

	curlAddHeaders(curl, request);

	curl_easy_setopt(curl, CURLOPT_URL, request->buildURL().c_str());

resend:
	cres = curl_easy_perform(curl);

	if (cres != CURLE_OK) {
		getResultQueue().push(new FailedQueueData(request->success, request->failed, curl_easy_strerror(cres)));
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
		redirect = "";

		goto resend;
	}

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response->code);

	getResultQueue().push(new SuccessQueueData(request->success, request->failed, response));

cleanup:
	// Clear out the HTTPResponse if we don't need it
	if (!ret) {
		delete response;
	}

	if (curl)
		curl_easy_cleanup(curl);

	return ret;
}

/*
 * See https://wiki.facepunch.com/gmod/Global.HTTP for documentation.
 * The function takes a single table argument, based off the HTTPRequest structure.
 * It returns a boolean whether a request was sent or not.
 */
LUA_FUNCTION(CHTTP) {
	if (!LUA->IsType(1, GarrysMod::Lua::Type::Table)) {
		return 0;
	}

	auto *request = new HTTPRequest();
	bool ret;

	// Fetch failed handler
	LUA->GetField(1, "failed");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
		request->failed = LUA->ReferenceCreate();
	} else {
		LUA->Pop();
	}

	// Fetch method
	LUA->GetField(1, "method");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->method = HTTPMethod::fromString(LUA->GetString(-1));
	}
	if (request->method == HTTPMethod::INVALID) {
		getResultQueue().push(new FailedQueueData(request->success, request->failed, "Unsupported request method: " + std::string(LUA->GetString(-1))));
		ret = false;
		goto exit;
	}
	LUA->Pop();

	// Fetch url
	LUA->GetField(1, "url");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->url = LUA->GetString(-1);
	} else {
		getResultQueue().push(new FailedQueueData(request->success, request->failed, "invalid url"));
		ret = false;
		goto exit;
	}
	LUA->Pop();

	// Fetch success handler
	LUA->GetField(1, "success");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
		request->success = LUA->ReferenceCreate();
	} else {
		LUA->Pop();
	}

	// Fetch headers
	LUA->GetField(1, "headers");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Table)) {
		luaTableToMap(LUA, -1, request->headers);
	}
	LUA->Pop();

	// Fetch parameters
	LUA->GetField(1, "parameters");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::Table)) {
		luaTableToMap(LUA, -1, request->parameters);
	}
	LUA->Pop();

	// Fetch type
	LUA->GetField(1, "type");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->headers["Content-Type"] = LUA->GetString(-1);
	}
	LUA->Pop();

	// Fetch body
	LUA->GetField(1, "body");
	if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
		request->body = LUA->GetString(-1);
	}
	LUA->Pop();

	ret = scheduleRequest(request);

exit:
	if(!ret)
		delete request;

	LUA->PushBool(ret); // Push result to the stack
	return 1; // We are returning a single value
}

LUA_FUNCTION(threadingDoThink) {
	while (!getResultQueue().empty()) {
		ResultQueueData* data = getResultQueue().front();
		getResultQueue().pop();
		data->run(LUA);
		delete data;
	}

	return 0;
}

GMOD_MODULE_OPEN() {
#ifdef WINDOWS_BUILD
	if (curl_global_sslset(CURLSSLBACKEND_SCHANNEL, nullptr, nullptr) != CURLSSLSET_OK) {
		LOG("error: The WinSSL/schannel backend is not available!");
		return 1;
	}
#endif

	// Initialize curl
	curl_global_init(CURL_GLOBAL_ALL);

	// We are working on the global table today
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

	// Push the function mapping (first is the key/function name,
	// second is the value/actual function)
	LUA->PushString("CHTTP");
	LUA->PushCFunction(CHTTP);

	// SetTable takes the item at the top of the stack (value) and
	// the second item from the top (key) and adds them to the table
	// at the stack offset mentioned in the parameter (again, -1 is the top)
	LUA->SetTable(-3);


	// Get the hook.Add method
	LUA->GetField(-1, "hook");
	LUA->GetField(-1, "Add");

	// Push the new hook data
	LUA->PushString("Think");
	LUA->PushString("__chttpThinkHook");
	LUA->PushCFunction(threadingDoThink);

	// Add the hook
	LUA->Call(3, 0);

	// Pop the "hook" table
	LUA->Pop();

	// Pop the global table from the stack again
	LUA->Pop();

	return 0;
}

GMOD_MODULE_CLOSE() {
	// Cleanup curl
	curl_global_cleanup();

	return 0;
}
