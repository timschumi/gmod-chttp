#pragma once

#include <cstdarg>
#include <string>
#include <vector>

class Logger {
public:
    static bool init();

    static void msg(char const* fmt, ...);

    static void warn(char const* fmt, ...);

    static void devmsg(char const* fmt, ...);

    static void devwarn(char const* fmt, ...);

private:
    static std::vector<char> format(char const* fmt, std::va_list args);

    static void msg_Impl(char const* str);

    static void warn_Impl(char const* str);

    static void devmsg_Impl(char const* str);

    static void devwarn_Impl(char const* str);

    static void (*msg_func)(char const*, ...);

    static void (*warn_func)(char const*, ...);

    static void (*devmsg_func)(int, char const*, ...);

    static void (*devwarn_func)(int, char const*, ...);
};
