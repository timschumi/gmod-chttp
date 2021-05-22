#pragma once

#include <string>
#include <map>

#include "HTTPMethod.h"

// Modeled after GMod's HTTPRequest structure
// https://wiki.facepunch.com/gmod/Structures/HTTPRequest
class HTTPRequest {
public:
	// Handler for failed requests. args: (string) reason
	// This is a reference to the function on the stack
	int failed = 0;

	// Handler for successful requests. args: (number) code, (string) body, (table) headers
	// This is a reference to the function on the stack
	int success = 0;

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

	HTTPRequest();
	std::string buildQueryString();
	std::string buildURL();
	bool run();
};
