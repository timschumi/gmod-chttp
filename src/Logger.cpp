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

bool Logger::init() {
	msg_func = reinterpret_cast<decltype(msg_func)>(getExport("tier0", "Msg"));

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
