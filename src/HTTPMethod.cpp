#include "HTTPMethod.h"

bool HTTPMethod::is_like_post()
{
    switch (this->m_method) {
    case HTTPMethod::Post:
    case HTTPMethod::Put:
    case HTTPMethod::Delete:
    case HTTPMethod::Patch:
        return true;
    default:
        return false;
    }
}

std::string HTTPMethod::to_string()
{
    switch (this->m_method) {
    case HTTPMethod::Get:
        return "GET";
    case HTTPMethod::Post:
        return "POST";
    case HTTPMethod::Head:
        return "HEAD";
    case HTTPMethod::Put:
        return "PUT";
    case HTTPMethod::Delete:
        return "DELETE";
    case HTTPMethod::Patch:
        return "PATCH";
    case HTTPMethod::Options:
        return "OPTIONS";
    default:
        return "INVALID";
    }
}

HTTPMethod HTTPMethod::from_string(std::string method)
{
    for (auto& c : method) {
        c = std::toupper(c);
    }

    if (method == "GET")
        return HTTPMethod::Get;
    if (method == "POST")
        return HTTPMethod::Post;
    if (method == "HEAD")
        return HTTPMethod::Head;
    if (method == "PUT")
        return HTTPMethod::Put;
    if (method == "DELETE")
        return HTTPMethod::Delete;
    if (method == "PATCH")
        return HTTPMethod::Patch;
    if (method == "OPTIONS")
        return HTTPMethod::Options;

    return HTTPMethod::Invalid;
}
