#pragma once

#include <string>

class HTTPMethod {
public:
    enum Method {
        Invalid = 0,
        Get,
        Head,
        Post,
        Put,
        Delete,
        Options,
        Patch,
    };

    HTTPMethod() = default;

    constexpr HTTPMethod(Method m)
        : m_method(m) {};

    operator Method() const
    {
        return m_method;
    }

    explicit operator bool() = delete;

    bool is_like_post();

    std::string to_string();

    static HTTPMethod from_string(std::string method);

private:
    Method m_method;
};
