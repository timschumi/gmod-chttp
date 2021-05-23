#pragma once

#include <cstdarg>
#include <string>

class Logger {
public:
	static bool init();

	static void msg(const char *fmt, ...);

	static void warn(const char *fmt, ...);

	static void devmsg(const char *fmt, ...);

	static void devwarn(const char *fmt, ...);

private:
	static const char *format(const char *fmt, std::va_list args);

	static void msg_Impl(const char *str);

	static void warn_Impl(const char *str);

	static void devmsg_Impl(const char *str);

	static void devwarn_Impl(const char *str);

	static void (*msg_func)(const char *, ...);

	static void (*warn_func)(const char *, ...);

	static void (*devmsg_func)(int, const char *, ...);

	static void (*devwarn_func)(int, const char *, ...);
};
