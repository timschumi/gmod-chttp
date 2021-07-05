#pragma once

#include <string>
#include <map>
#include <memory>

#include "HTTPMethod.h"
#include "LuaReference.h"

// Modeled after GMod's HTTPRequest structure
// https://wiki.facepunch.com/gmod/Structures/HTTPRequest
class HTTPRequest {
public:
	// Handler for failed requests. args: (string) reason
	std::shared_ptr<LuaReference> failed = nullptr;

	// Handler for successful requests. args: (number) code, (string) body, (table) headers
	std::shared_ptr<LuaReference> success = nullptr;

	// Request method for the request
	HTTPMethod method = HTTPMethod::M_GET;

	// Well, what could that be?
	std::string url;

	// Key-Value table for GET, POST, and HEAD request parameters.
	std::map<std::string, std::string> parameters;

	// Key-Value table for headers.
	std::map<std::string, std::string> headers;

	// Request body for POST-like requests (overwrites parameters)
	std::string body;

	// Request body type for POST-like requests
	std::string type = "text/plain; charset=utf-8";

	HTTPRequest();

	std::string buildQueryString();

	std::string buildURL();

	bool run();
};
