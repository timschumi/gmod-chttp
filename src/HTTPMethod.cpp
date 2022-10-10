#include "HTTPMethod.h"

bool HTTPMethod::isLikePost()
{
    switch (this->method) {
    case HTTPMethod::M_POST:
    case HTTPMethod::M_PUT:
    case HTTPMethod::M_DELETE:
    case HTTPMethod::M_PATCH:
        return true;
    default:
        return false;
    }
}

std::string HTTPMethod::toString()
{
    switch (this->method) {
    case HTTPMethod::M_GET:
        return "GET";
    case HTTPMethod::M_POST:
        return "POST";
    case HTTPMethod::M_HEAD:
        return "HEAD";
    case HTTPMethod::M_PUT:
        return "PUT";
    case HTTPMethod::M_DELETE:
        return "DELETE";
    case HTTPMethod::M_PATCH:
        return "PATCH";
    case HTTPMethod::M_OPTIONS:
        return "OPTIONS";
    default:
        return "INVALID";
    }
}

HTTPMethod HTTPMethod::fromString(std::string method)
{
    for (auto& c : method) {
        c = std::toupper(c);
    }

    if (method == "GET")
        return HTTPMethod::M_GET;
    if (method == "POST")
        return HTTPMethod::M_POST;
    if (method == "HEAD")
        return HTTPMethod::M_HEAD;
    if (method == "PUT")
        return HTTPMethod::M_PUT;
    if (method == "DELETE")
        return HTTPMethod::M_DELETE;
    if (method == "PATCH")
        return HTTPMethod::M_PATCH;
    if (method == "OPTIONS")
        return HTTPMethod::M_OPTIONS;

    return HTTPMethod::M_INVALID;
}
