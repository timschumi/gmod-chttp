#pragma once

#include <map>
#include <memory>
#include <string>

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
    HTTPMethod method = HTTPMethod::Get;

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

    // Timeout for the request
    long timeout = 60;

    // Optionally prevent redirects (30x)
    // NOTE: Not in GMod HTTP
    bool noredirect = false;

    HTTPRequest();

    std::string build_query_string();

    std::string build_url();

    bool run();
};
