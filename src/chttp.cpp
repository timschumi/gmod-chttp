#include <curl/curl.h>
#include <string>

#include "GarrysMod/Lua/Interface.h"

#include "Logger.h"
#include "RequestWorker.h"
#include "lua.h"

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

    auto request = std::make_shared<HTTPRequest>();
    std::string failreason;

    // Fetch failed handler
    LUA->GetField(1, "failed");
    if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
        request->failed = std::make_shared<LuaReference>(LUA);
    } else {
        LUA->Pop();
    }

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

    RequestWorker::the().requests().push(request);

exit:
    if (!failreason.empty()) {
        RequestWorker::the().tasks().push(std::make_shared<FailCallbackTask>(request->failed, failreason));
    }

    LUA->PushBool(true); // Push result to the stack
    return 1;            // We are returning a single value
}

LUA_FUNCTION(lua_run_tasks)
{
    RequestWorker::the().run_tasks(LUA);

    return 0;
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

    return 0;
}
