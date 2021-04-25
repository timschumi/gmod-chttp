#ifndef GMOD_CHTTP_LOGGER_H
#define GMOD_CHTTP_LOGGER_H

#include <cstdarg>
#include <string>

class Logger {
public:
	static bool init();

	static void msg(std::string fmt, ...);

private:
	static std::string format(const std::string& fmt, std::va_list args);

	static void (*msg_func)(const char *, ...);
};


#endif
