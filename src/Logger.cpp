#include "Logger.h"

#ifdef _WIN32
#    include <libloaderapi.h>
#else
#    include <dlfcn.h>
#endif

static void* get_export(std::string const& library, std::string const& symbol)
{
#ifdef _WIN32
    HMODULE handle = GetModuleHandle((library + ".dll").c_str());
    return reinterpret_cast<void*>(GetProcAddress(handle, symbol.c_str()));
#else
    void* handle = dlopen(("lib" + library + ".so").c_str(), RTLD_LAZY);
    return dlsym(handle, symbol.c_str());
#endif
}

static bool is_library_loaded(std::string const& library)
{
#ifdef _WIN32
    return GetModuleHandle((library + ".dll").c_str());
#else
    return dlopen(("lib" + library + ".so").c_str(), RTLD_LAZY | RTLD_NOLOAD);
#endif
}

decltype(Logger::msg_func) Logger::msg_func = nullptr;
decltype(Logger::warn_func) Logger::warn_func = nullptr;
decltype(Logger::devmsg_func) Logger::devmsg_func = nullptr;
decltype(Logger::devwarn_func) Logger::devwarn_func = nullptr;

bool Logger::init()
{
    std::string tier0_name = "";

    std::string tier0_list[] = {
#ifdef __linux__
        "tier0_client",
        "tier0_srv",
#endif
        "tier0",
    };

    // Find the first library in the list that is already loaded.
    for (std::string& lib : tier0_list) {
        if (is_library_loaded(lib)) {
            tier0_name = lib;
            break;
        }
    }

    if (tier0_name.empty()) {
        return false;
    }

    msg_func = reinterpret_cast<decltype(msg_func)>(get_export(tier0_name, "Msg"));
    warn_func = reinterpret_cast<decltype(warn_func)>(get_export(tier0_name, "Warning"));
    devmsg_func = reinterpret_cast<decltype(devmsg_func)>(get_export(tier0_name, "DevMsg"));
    devwarn_func = reinterpret_cast<decltype(devwarn_func)>(get_export(tier0_name, "DevWarning"));

    return msg_func && warn_func && devmsg_func && devwarn_func;
}

std::vector<char> Logger::format(char const* fmt, std::va_list args)
{
    // Copy args list so that we can use it twice
    std::va_list args_copy;
    va_copy(args_copy, args);

    // Determine length of formatted string and create a memory location for it
    int length = std::vsnprintf(nullptr, 0, fmt, args);
    std::vector<char> buf(length + 1);

    // Actually generate the string
    std::vsnprintf(&buf[0], length + 1, fmt, args_copy);

    va_end(args_copy);
    return buf;
}

#define FMT_WRAP(FUNC)                        \
    void FUNC(char const* fmt, ...)           \
    {                                         \
        std::va_list args;                    \
        va_start(args, fmt);                  \
        auto buf = Logger::format(fmt, args); \
        va_end(args);                         \
                                              \
        FUNC##_Impl(&buf[0]);                 \
    }                                         \
    void FUNC##_Impl(char const* str)

FMT_WRAP(Logger::msg)
{
    if (Logger::msg_func)
        Logger::msg_func("[chttp] %s\n", str);
}

FMT_WRAP(Logger::warn)
{
    if (Logger::warn_func)
        Logger::warn_func("[chttp] %s\n", str);
}

FMT_WRAP(Logger::devmsg)
{
    if (Logger::devmsg_func)
        Logger::devmsg_func(1, "[chttp] %s\n", str);
}

FMT_WRAP(Logger::devwarn)
{
    if (Logger::devwarn_func)
        Logger::devwarn_func(1, "[chttp] %s\n", str);
}
