#ifndef GMOD_CHTTP_HTTPREQUEST_H
#define GMOD_CHTTP_HTTPREQUEST_H

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
	HTTPMethod method = HTTPMethod::GET;

	// Well, what could that be?
	std::string url;

	// Key-Value table for GET, POST, and HEAD request parameters.
	std::map<std::string, std::string> parameters;

	// Key-Value table for headers.
	std::map<std::string, std::string> headers;

	// Request body for POST-like requests (overwrites parameters)
	std::string body;

	// Content-Type string for the request body.
	std::string type = "text/plain; charset=utf-8";

	std::string buildQueryString();
	std::string buildURL();
};

#endif
