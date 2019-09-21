#include <map>
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

/*
 * See https://wiki.garrysmod.com/page/Global/HTTP for documentation.
 * The function takes a single table argument, based off the HTTPRequest structure.
 * It returns a boolean whether a request was sent or not.
 */
int CHTTP(lua_State *state) {
	HTTPRequest request = HTTPRequest();

	LOG("Called HTTP()! STUB!");

	LUA->CheckType(1, Lua::Type::TABLE);

	dumpRequest(state, request);

	LUA->PushBool(true); // Push result to the stack
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
