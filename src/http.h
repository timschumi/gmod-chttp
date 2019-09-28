#include <map>
#include <GarrysMod/Lua/LuaBase.h>

// Modeled after GMod's HTTPRequest structure
// https://wiki.garrysmod.com/page/Structures/HTTPRequest
struct HTTPRequest {
	// Handler for failed requests. args: (string) reason
	GarrysMod::Lua::CFunc failed;

	// Handler for successful requests. args: (number) code, (string) body, (table) headers
	GarrysMod::Lua::CFunc success;

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
