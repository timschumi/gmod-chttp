#pragma once

#include <cstdarg>
#include <string>

class Logger {
public:
	static bool init();

	static void msg(std::string fmt, ...);

	static void warn(std::string fmt, ...);

	static void devmsg(std::string fmt, ...);

	static void devwarn(std::string fmt, ...);

private:
	static std::string format(const std::string &fmt, std::va_list args);

	static void (*msg_func)(const char *, ...);

	static void (*warn_func)(const char *, ...);

	static void (*devmsg_func)(int, const char *, ...);

	static void (*devwarn_func)(int, const char *, ...);
};
