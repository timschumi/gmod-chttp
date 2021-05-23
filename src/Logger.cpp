#include "Logger.h"

#ifdef _WIN32
#include <libloaderapi.h>
#else

#include <dlfcn.h>

#endif

static void *getExport(const std::string &library, const std::string &symbol) {
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
	std::string tier0_name = "tier0";

#if defined(__linux__) && defined(__x86_64__)
	// Check if tier0_client is already loaded.
	// If so, it's likely that we want that instead of the "server" tier0.
	if (dlopen("libtier0_client.so", RTLD_LAZY | RTLD_NOLOAD))
		tier0_name = "tier0_client";
#endif

	msg_func = reinterpret_cast<decltype(msg_func)>(getExport(tier0_name, "Msg"));
	warn_func = reinterpret_cast<decltype(warn_func)>(getExport(tier0_name, "Warning"));
	devmsg_func = reinterpret_cast<decltype(devmsg_func)>(getExport(tier0_name, "DevMsg"));
	devwarn_func = reinterpret_cast<decltype(devwarn_func)>(getExport(tier0_name, "DevWarning"));

	return msg_func && warn_func && devmsg_func && devwarn_func;
}

const char *Logger::format(const char *fmt, std::va_list args) {
	// Copy args list so that we can use it twice
	std::va_list args_copy;
	va_copy(args_copy, args);

	// Determine length of formatted string and create a memory location for it
	int length = std::vsnprintf(nullptr, 0, fmt, args);
	char *data = new char[length + 1];

	// Actually generate the string
	std::vsnprintf(data, length + 1, fmt, args_copy);

	va_end(args_copy);
	return data;
}

#define FMT_WRAP(FUNC)                        \
	void FUNC(const char *fmt, ...) {         \
		std::va_list args;                    \
		va_start(args, fmt);                  \
		auto str = Logger::format(fmt, args); \
		va_end(args);                         \
                                              \
		FUNC##_Impl(str);                     \
                                              \
		delete[] str;                         \
	}                                         \
	void FUNC##_Impl(const char *str)

FMT_WRAP(Logger::msg) {
	if (Logger::msg_func)
		Logger::msg_func("[chttp] %s\n", str);
}

FMT_WRAP(Logger::warn) {
	if (Logger::warn_func)
		Logger::warn_func("[chttp] %s\n", str);
}

FMT_WRAP(Logger::devmsg) {
	if (Logger::devmsg_func)
		Logger::devmsg_func(1, "[chttp] %s\n", str);
}

FMT_WRAP(Logger::devwarn) {
	if (Logger::devwarn_func)
		Logger::devwarn_func(1, "[chttp] %s\n", str);
}
