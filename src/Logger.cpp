#include <vector>
#include "Logger.h"

#ifdef _WIN32
#include <libloaderapi.h>
#else
#include <dlfcn.h>
#endif


static void *getExport(const std::string& library, const std::string& symbol) {
#ifdef _WIN32
	HMODULE handle = GetModuleHandle((library + ".dll").c_str());
	return reinterpret_cast<void *>(GetProcAddress(handle, symbol.c_str()));
#else
	void *handle = dlopen(("lib" + library + ".so").c_str(), RTLD_LAZY);
	return dlsym(handle, symbol.c_str());
#endif
}

decltype(Logger::msg_func) Logger::msg_func = nullptr;
decltype(Logger::warn_func) Logger::warn_func = nullptr;
decltype(Logger::devmsg_func) Logger::devmsg_func = nullptr;
decltype(Logger::devwarn_func) Logger::devwarn_func = nullptr;

bool Logger::init() {
	msg_func = reinterpret_cast<decltype(msg_func)>(getExport("tier0", "Msg"));
	warn_func = reinterpret_cast<decltype(warn_func)>(getExport("tier0", "Warning"));
	devmsg_func = reinterpret_cast<decltype(devmsg_func)>(getExport("tier0", "DevMsg"));
	devwarn_func = reinterpret_cast<decltype(devwarn_func)>(getExport("tier0", "DevWarning"));

	return msg_func;
}

std::string Logger::format(const std::string& fmt, std::va_list args) {
	// Copy args list so that we can use it twice
	std::va_list args_copy;
	va_copy(args_copy, args);

	// Determine length of formatted string and create a vector for it
	int length = std::vsnprintf(nullptr, 0, fmt.c_str(), args);
	std::vector<char> data(length + 1);

	// Actually generate the string
	std::vsnprintf(data.data(), data.size(), fmt.c_str(), args_copy);

	va_end(args_copy);
	return std::string(data.data(), data.size());
}

void Logger::msg(std::string fmt, ...) {
	std::va_list args;
	va_start(args, fmt);
	auto formatted_string = Logger::format(fmt, args);
	va_end(args);

	if (Logger::msg_func)
		Logger::msg_func("[chttp] %s\n", formatted_string.c_str());
}

void Logger::warn(std::string fmt, ...) {
	std::va_list args;
	va_start(args, fmt);
	auto formatted_string = Logger::format(fmt, args);
	va_end(args);

	if (Logger::warn_func)
		Logger::warn_func("[chttp] %s\n", formatted_string.c_str());
}

void Logger::devmsg(std::string fmt, ...) {
	std::va_list args;
	va_start(args, fmt);
	auto formatted_string = Logger::format(fmt, args);
	va_end(args);

	if (Logger::devmsg_func)
		Logger::devmsg_func(1, "[chttp] %s\n", formatted_string.c_str());
}

void Logger::devwarn(std::string fmt, ...) {
	std::va_list args;
	va_start(args, fmt);
	auto formatted_string = Logger::format(fmt, args);
	va_end(args);

	if (Logger::devwarn_func)
		Logger::devwarn_func(1, "[chttp] %s\n", formatted_string.c_str());
}
