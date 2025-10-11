#include <algorithm>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <string>
#ifdef __linux__
#    include <sys/utsname.h>
#endif

#include "GarrysMod/Lua/Interface.h"

#include "Logger.h"
#include "RequestWorker.h"
#include "lua.h"

LUA_FUNCTION(lua_run_tasks)
{
    RequestWorker::the().run_tasks(LUA);

    return 0;
}

std::string handle_request_hook(GarrysMod::Lua::ILuaBase* LUA, int request_index)
{
    std::string result;

    int initial_top = LUA->Top();
    LUA->ReferencePush(lua_ref_hook_Run);
    LUA->PushString("OnCHTTPRequest");
    LUA->Push(request_index < 0 ? request_index - 2 : request_index);
    LUA->Call(2, -1);
    int number_of_values = LUA->Top() - initial_top;

    // Prune away extraneous returned nil values.
    // hook.Call has great code quality and is without faults.
    while (number_of_values > 0 && LUA->IsType(-1, GarrysMod::Lua::Type::Nil)) {
        LUA->Pop();
        number_of_values--;
    }

    // Hook returned a fail reason.
    if (number_of_values == 1 && LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
        unsigned int failreason_length;
        char const* failreason = LUA->GetString(-1, &failreason_length);
        result = std::string(failreason, failreason_length);
        if (result.empty())
            result = "request rejected by hook";
    }

    LUA->Pop(number_of_values);

    return result;
}

/*
 * See https://wiki.facepunch.com/gmod/Global.HTTP for documentation.
 * The function takes a single table argument, based off the HTTPRequest structure.
 * It returns a boolean whether a request was sent or not.
 */
LUA_FUNCTION(CHTTP)
{
    if (!LUA->IsType(1, GarrysMod::Lua::Type::Table)) {
        return 0;
    }

    std::string failreason = handle_request_hook(LUA, -1);
    auto request = std::make_shared<HTTPRequest>();

    // Fetch failed handler
    LUA->GetField(1, "failed");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
        request->failed = std::make_shared<LuaReference>(LUA);
    } else {
        LUA->Pop();
    }

    if (!failreason.empty())
        goto exit;

    // Fetch method
    LUA->GetField(1, "method");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
        request->method = HTTPMethod::from_string(LUA->GetString(-1));
    }
    LUA->Pop();

    // Fetch url
    LUA->GetField(1, "url");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
        request->url = LUA->GetString(-1);
    } else {
        failreason = "invalid url";
        goto exit;
    }
    LUA->Pop();

    // Fetch success handler
    LUA->GetField(1, "success");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
        request->success = std::make_shared<LuaReference>(LUA);
    } else {
        LUA->Pop();
    }

    // Fetch headers
    LUA->GetField(1, "headers");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::Table)) {
        lua_table_to_map(LUA, -1, request->headers);
    }
    LUA->Pop();

    // Fetch parameters
    LUA->GetField(1, "parameters");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::Table)) {
        lua_table_to_map(LUA, -1, request->parameters);
    }
    LUA->Pop();

    // Fetch type
    LUA->GetField(1, "type");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
        request->type = LUA->GetString(-1);
    }
    LUA->Pop();

    // Fetch body
    LUA->GetField(1, "body");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::String)) {
        unsigned int body_length;
        char const* body = LUA->GetString(-1, &body_length);
        request->body = std::string(body, body_length);
    }
    LUA->Pop();

    // Fetch timeout
    LUA->GetField(1, "timeout");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::Number)) {
        request->timeout = (long)LUA->GetNumber(-1);
    }
    LUA->Pop();

    // Fetch noredirect toggle
    LUA->GetField(1, "noredirect");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::Bool)) {
        request->noredirect = (bool)LUA->GetBool(-1);
    }
    LUA->Pop();

    if (!getenv("CHTTP_FORCE_HOOK")) {
        // If we are using timers, ensure that the timer is still present.
        // It might have gotten destroyed if there was an exception while running a callback.
        LUA->ReferencePush(lua_ref_timer_Exists);
        LUA->PushString("__chttpThinkTimer");
        LUA->Call(1, 1);
        auto exists = LUA->GetBool(-1);
        LUA->Pop();

        if (!exists) {
            Logger::msg("Recreating timer for processing requests...");
            register_zero_delay_timer(LUA, "__chttpThinkTimer", lua_run_tasks);
        }
    }

    RequestWorker::the().requests().push(request);

exit:
    if (!failreason.empty()) {
        RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(request->failed, failreason));
    }

    LUA->PushBool(true); // Push result to the stack
    return 1;            // We are returning a single value
}

LUA_FUNCTION(handle_update_response)
{
    double code = LUA->GetNumber(1);

    // Both code 204 (no update) and all other codes (server down?) will be ignored silently.
    if (code != 200)
        return 0;

    std::map<std::string, std::string> headers;
    lua_table_to_map(LUA, 3, headers);

    // Treat responses without the magic header as malformed.
    std::string version_header_name = "Chttp-Update-Message-Version";
    if (headers.count(version_header_name) == 0) {
        version_header_name = "chttp-update-message-version";
        if (headers.count(version_header_name) == 0)
            return 0;
    }

    auto version = std::stoi(headers[version_header_name]);
    if (version < 1)
        return 0;

    if (version > 1) {
        Logger::warn("The update server responded with a newer-than-supported message version.");
        Logger::warn("Please manually check the GitHub page for new updates:");
        Logger::warn("https://github.com/timschumi/gmod-chttp/releases");
        return 0;
    }

    unsigned int raw_body_length;
    char const* raw_body = LUA->GetString(2, &raw_body_length);
    std::string body(raw_body, raw_body_length);

    std::istringstream body_stream(body);
    std::string line;

    while (std::getline(body_stream, line)) {
        Logger::warn("%s", line.c_str());
    }

    return 0;
}

void handle_updates_or_telemetry(GarrysMod::Lua::ILuaBase* LUA)
{
    bool disable_update_notification = getenv("CHTTP_DISABLE_UPDATE_NOTIFICATION");
    bool disable_telemetry = getenv("CHTTP_DISABLE_TELEMETRY");

    // No need to do anything if we don't want update information and don't want to check in.
    if (disable_update_notification && disable_telemetry)
        return;

    auto request = std::make_shared<HTTPRequest>();
    request->method = HTTPMethod::Post;
    request->url = "https://chttp.timschumi.net/checkin";
    request->timeout = 5;

    if (!disable_update_notification) {
        LUA->PushCFunction(handle_update_response);
        request->success = std::make_shared<LuaReference>(LUA);
    }

    request->parameters["version"] = CHTTP_VERSION;
    if (!disable_telemetry) {
        request->parameters["build_target"] = CHTTP_BUILD_TARGET;
        request->parameters["build_type"] = CHTTP_BUILD_TYPE;
        request->parameters["build_static"] = CHTTP_BUILD_STATIC;

#ifdef __linux__
        struct utsname utsname { };
        if (uname(&utsname) >= 0) {
            // As much as I'd like to for deduplication purposes, the nodename (i.e. hostname) is not sent.
            request->parameters["os_sysname"] = utsname.sysname;
            request->parameters["os_release"] = utsname.release;
            request->parameters["os_version"] = utsname.version;
            request->parameters["os_machine"] = utsname.machine;
        }

        std::ifstream os_release_file("/etc/os-release");
        std::string os_release_line;

        while (std::getline(os_release_file, os_release_line)) {
            std::string key;
            std::string value;

            if (os_release_line.rfind("ID=", 0) == 0) {
                key = "dist_name";
                value = os_release_line.substr(3);
            } else if (os_release_line.rfind("VERSION_ID=", 0) == 0) {
                key = "dist_version";
                value = os_release_line.substr(11);
            } else {
                continue;
            }

            // Strip quotes from the value.
            value.erase(std::remove(value.begin(), value.end(), '"'), value.end());

            request->parameters[key] = value;
        }
#endif
    }

    RequestWorker::the().requests().push(request);
}

GMOD_MODULE_OPEN()
{
    // Set up logging
    if (!Logger::init()) {
        LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->GetField(-1, "print");
        LUA->PushString(("[chttp] Could not get all required logging functions. Most messages will not be shown."));
        LUA->Call(1, 0);
        LUA->Pop();
    }

    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);

    // We are working on the global table today
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);

    // Obtain some function references.
    LUA->GetField(-1, "hook");
    LUA->GetField(-1, "Add");
    lua_ref_hook_Add = LUA->ReferenceCreate();
    LUA->GetField(-1, "Run");
    lua_ref_hook_Run = LUA->ReferenceCreate();
    LUA->Pop();
    LUA->GetField(-1, "timer");
    LUA->GetField(-1, "Create");
    lua_ref_timer_Create = LUA->ReferenceCreate();
    LUA->GetField(-1, "Exists");
    lua_ref_timer_Exists = LUA->ReferenceCreate();
    LUA->Pop();

    // Push the function mapping (first is the key/function name,
    // second is the value/actual function)
    LUA->PushString("CHTTP");
    LUA->PushCFunction(CHTTP);

    // SetTable takes the item at the top of the stack (value) and
    // the second item from the top (key) and adds them to the table
    // at the stack offset mentioned in the parameter (again, -1 is the top)
    LUA->SetTable(-3);

    LUA->PushString("CHTTP_VERSION");
    LUA->PushString(CHTTP_VERSION);
    LUA->SetTable(-3);

    // Pop the global table from the stack again
    LUA->Pop();

    if (getenv("CHTTP_FORCE_HOOK")) {
        Logger::msg("Processing requests using a hook...");
        register_hook(LUA, "Think", "__chttpThinkHook", lua_run_tasks);
    } else {
        Logger::msg("Processing requests using a zero-delay timer...");
        register_zero_delay_timer(LUA, "__chttpThinkTimer", lua_run_tasks);
    }

    // Start the background thread
    RequestWorker::the();

    handle_updates_or_telemetry(LUA);

    return 0;
}

GMOD_MODULE_CLOSE()
{
    // Process remaining requests and callbacks
    while (RequestWorker::the().has_work()) {
        RequestWorker::the().run_tasks(LUA);
    }

    // Stop the request worker
    RequestWorker::the().stop();

    // Cleanup curl
    curl_global_cleanup();

    LUA->ReferenceFree(lua_ref_hook_Add);
    LUA->ReferenceFree(lua_ref_hook_Run);
    LUA->ReferenceFree(lua_ref_timer_Create);
    LUA->ReferenceFree(lua_ref_timer_Exists);

    return 0;
}
