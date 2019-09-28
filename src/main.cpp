#include <curl/curl.h>
#include "GarrysMod/Lua/Interface.h"
#include "http.h"

#define LOG(x) printMessage(LUA, x);

using namespace GarrysMod;

std::string buildUserAgent() {
	std::string user = "";
	curl_version_info_data *info = curl_version_info(CURLVERSION_NOW);

	user += "User-Agent:";

	user += " curl/";
	user += info->version;

	user += " gmod-chttp/0";

	return user;
}

static void printMessage(GarrysMod::Lua::ILuaBase *LUA, std::string message) {
	// Push global table to the stack to work on it
	LUA->PushSpecial(Lua::SPECIAL_GLOB);

	// Gets the print function and stores it at the top of the stack (top = -1)
	LUA->GetField(-1, "print");

	// Pushes the argument to the stack
	LUA->PushString(("[CHTTP] " + message).c_str());

	// Calls the function (arg1 = number of arguments, arg2 = number of return values).
	// The function is the top of the stack after arguments have been popped.
	// In this case, we are calling with 1 argument (the message) and no return values (because it's `print`, duh!)
	LUA->Call(1, 0);

	// Pops the last value from the stack (the global table?)
	LUA->Pop();
}

void dumpRequest(GarrysMod::Lua::ILuaBase *LUA, HTTPRequest request) {
	LOG("Dumping request:");
	LOG("url: " + request.url);
	LOG("method: " + request.method);
	LOG("headers: [" + std::to_string(request.headers.size()) + "]");
	for (auto const& e : request.headers) {
		LOG("  " + e.first + ": " + e.second);
	}
}

void requestFailed(GarrysMod::Lua::ILuaBase *LUA, HTTPRequest request, std::string reason) {
	// The request doesn't have a failure handler attached,
	// so just print the error in the log.
	if (!request.failed) {
		printMessage(LUA, "[requestFailed] reason: " + reason);
		return;
	}

	// Push fail handler to stack
	LUA->PushCFunction(request.failed);

	// Push the argument
	LUA->PushString(reason.c_str());

	// Call the fail handler with one argument
	LUA->Call(1, 0);
}

bool processRequest(GarrysMod::Lua::ILuaBase *LUA, HTTPRequest request) {
	CURL *curl;
	CURLcode cres;
	bool ret = true;
	struct curl_slist *headers = NULL;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	if (!curl) {
		requestFailed(LUA, request, "Failed to init curl struct!");
		ret = false;
		goto global_cleanup;
	}

	curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

	headers = curl_slist_append(headers, buildUserAgent().c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	cres = curl_easy_perform(curl);

	if (cres != CURLE_OK) {
		requestFailed(LUA, request, curl_easy_strerror(cres));
		ret = false;
		goto cleanup;
	}

cleanup:
	curl_easy_cleanup(curl);

global_cleanup:
	curl_global_cleanup();
	return ret;
}

// Transfers values from a LUA Table on the stack (at the given offset)
// into a map that will be the return value.
std::map<std::string, std::string> mapFromLuaTable(GarrysMod::Lua::ILuaBase *LUA, int index) {
	std::map<std::string, std::string> map;

	// Query the first entry (we're querying by key, nil = 1st)
	LUA->PushNil();

	// ->Next() gets the last key from the stack and pushes
	// the key-value pair that follows that to the stack.
	// key will now be top-2 and value will be top-1
	while (LUA->Next(index - 1) != 0) {
		// Only store things with String keys
		if (LUA->IsType(-2, Lua::Type::STRING))
			map[LUA->GetString(-2)] = LUA->GetString(-1);

		// Pop value from the stack, key is needed for next iteration
		LUA->Pop();
	}

	return map;
}

/*
 * See https://wiki.garrysmod.com/page/Global/HTTP for documentation.
 * The function takes a single table argument, based off the HTTPRequest structure.
 * It returns a boolean whether a request was sent or not.
 */
LUA_FUNCTION(CHTTP) {
	HTTPRequest request = HTTPRequest();
	bool ret;

	LOG("Called HTTP()! STUB!");

	if (!LUA->IsType(1, Lua::Type::TABLE)) {
		LOG("No HTTPRequest table set.");
		ret = false;
		goto exit;
	}

	// Fetch failed handler
	LUA->GetField(1, "failed");
	if (LUA->IsType(-1, Lua::Type::FUNCTION)) {
		request.failed = LUA->GetCFunction(-1);
	}
	LUA->Pop();

	// Fetch method
	LUA->GetField(1, "method");
	if (LUA->IsType(-1, Lua::Type::STRING)) {
		request.method = LUA->GetString(-1);
	} else {
		request.method = "GET";
	}
	LUA->Pop();

	// Fetch url
	LUA->GetField(1, "url");
	if (LUA->IsType(-1, Lua::Type::STRING)) {
		request.url = LUA->GetString(-1);
	} else {
		requestFailed(LUA, request, "invalid url");
		ret = false;
		goto exit;
	}
	LUA->Pop();

	// Fetch success handler
	LUA->GetField(1, "success");
	if (LUA->IsType(-1, Lua::Type::FUNCTION)) {
		request.success = LUA->GetCFunction(-1);
	}
	LUA->Pop();

	// Fetch headers
	LUA->GetField(1, "headers");
	if (LUA->IsType(-1, Lua::Type::TABLE)) {
		request.headers = mapFromLuaTable(LUA, -1);
	}
	LUA->Pop();

	dumpRequest(LUA, request);
	ret = processRequest(LUA, request);

exit:
	LUA->PushBool(ret); // Push result to the stack
	return 1; // We are returning a single value
}

GMOD_MODULE_OPEN() {
	LOG("Loading CHTTP module.");

	// We are working on the global table today
	LUA->PushSpecial(Lua::SPECIAL_GLOB);

	// Push the function mapping (first is the key/function name,
	// second is the value/actual function)
	LUA->PushString("HTTP");
	LUA->PushCFunction(CHTTP);

	// SetTable takes the item at the top of the stack (value) and
	// the second item from the top (key) and adds them to the table
	// at the stack offset mentioned in the parameter (again, -1 is the top)
	LUA->SetTable(-3);

	// Pop the global table from the stack again
	LUA->Pop();

	LOG("Finished loading CHTTP module.");

	return 0;
}
