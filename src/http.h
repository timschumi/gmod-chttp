#ifndef _HTTP_H
#define _HTTP_H

#include <string>
#include <map>

// Modeled after GMod's HTTPRequest structure
// https://wiki.facepunch.com/gmod/Structures/HTTPRequest
struct HTTPRequest {
	// Handler for failed requests. args: (string) reason
	// This is a reference to the function on the stack
	int failed = 0;

	// Handler for successful requests. args: (number) code, (string) body, (table) headers
	// This is a reference to the function on the stack
	int success = 0;

	// Request method (GET = 1, POST = 2, etc.)
	// See method.{h,c} for details.
	int method;

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

// Not really modeled after anything specific
// These are just the values that we need for the success handler.
struct HTTPResponse {
	long code;
	std::string body;
	std::map<std::string, std::string> headers;
};

std::string buildParameters(HTTPRequest *request);
std::string buildUrl(HTTPRequest *request);

#endif