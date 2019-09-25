#include <map>
#include <curl/curl.h>
#include "GarrysMod/Lua/Interface.h"

#define LOG(x) printMessage(state, x);

using namespace GarrysMod;

struct HTTPRequest {
	// Handler for failed requests. args: (string) reason
	Lua::CFunc failed;

	// Handler for successful requests. args: (number) code, (string) body, (table) headers
	Lua::CFunc success;

	// Request method (GET, POST, etc.)
	std::string method;

	// Well, what could that be?
	std::string url;

	// Key-Value table for GET, POST, and HEAD request parameters.
	std::map<std::string, std::string> parameters;

	// Key-Value table for headers.
	std::map<std::string, std::string> headers;

	// Request body for POST-like requests (overwrites parameters)
	std::string body;

	// Content-Type string for the request body.
	std::string type;
};

static void printMessage(lua_State *state, std::string message) {
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

void dumpRequest(lua_State *state, HTTPRequest request) {
	LOG("Dumping request:");
	LOG("url: " + request.url);
	LOG("method: " + request.method);
}

void request_failed(lua_State *state, HTTPRequest request, std::string reason) {
	// The request doesn't have a failure handler attached,
	// so just print the error in the log.
	if (!request.failed) {
		printMessage(state, "[request_failed] reason: " + reason);
		return;
	}

	// Push fail handler to stack
	LUA->PushCFunction(request.failed);

	// Push the argument
	LUA->PushString(reason.c_str());

	// Call the fail handler with one argument
	LUA->Call(1, 0);
}

bool request_process(lua_State *state, HTTPRequest request) {
	CURL *curl;
	CURLcode cres;
	bool ret = true;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	if (!curl) {
		request_failed(state, request, "Failed to init curl struct!");
		ret = false;
		goto global_cleanup;
	}

	curl_easy_setopt(curl, CURLOPT_URL, request.url);
	cres = curl_easy_perform(curl);

	if (cres != CURLE_OK) {
		request_failed(state, request, curl_easy_strerror(cres));
		ret = false;
		goto cleanup;
	}

cleanup:
	curl_easy_cleanup(curl);

global_cleanup:
	curl_global_cleanup();
	return ret;
}

/*
 * See https://wiki.garrysmod.com/page/Global/HTTP for documentation.
 * The function takes a single table argument, based off the HTTPRequest structure.
 * It returns a boolean whether a request was sent or not.
 */
int CHTTP(lua_State *state) {
	HTTPRequest request = HTTPRequest();
	bool ret;

	LOG("Called HTTP()! STUB!");

	LUA->CheckType(1, Lua::Type::TABLE);

	// Fetch method
	LUA->GetField(1, "method");
	LUA->CheckType(-1, Lua::Type::STRING);
	request.method = LUA->GetString(-1);
	LUA->Pop();

	// Fetch url
	LUA->GetField(1, "url");
	LUA->CheckType(-1, Lua::Type::STRING);
	request.url = LUA->GetString(-1);
	LUA->Pop();

	// Fetch failed handler
	LUA->GetField(1, "failed");
	if (LUA->IsType(-1, Lua::Type::FUNCTION)) {
		request.failed = LUA->GetCFunction(-1);
	}
	LUA->Pop();

	// Fetch success handler
	LUA->GetField(1, "success");
	if (LUA->IsType(-1, Lua::Type::FUNCTION)) {
		request.success = LUA->GetCFunction(-1);
	}
	LUA->Pop();

	dumpRequest(state, request);
	ret = request_process(state, request);

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
