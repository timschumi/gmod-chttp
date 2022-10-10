#include "LuaTask.h"

#include <utility>

#include "Logger.h"
#include "lua.h"

RequestCallbackTask::RequestCallbackTask(std::shared_ptr<LuaReference> callback)
    : m_callback(std::move(callback))
{
}

SuccessCallbackTask::SuccessCallbackTask(std::shared_ptr<LuaReference> callback)
    : RequestCallbackTask(std::move(callback))
{
}

void SuccessCallbackTask::run(GarrysMod::Lua::ILuaBase* LUA)
{
    if (!this->m_callback) {
        return;
    }

    m_callback->push(LUA);
    LUA->PushNumber(this->code);
    LUA->PushString(this->body.c_str(), this->body.size());
    map_to_lua_table(LUA, this->headers);
    LUA->Call(3, 0);
}

FailCallbackTask::FailCallbackTask(std::shared_ptr<LuaReference> callback, std::string reason)
    : RequestCallbackTask(std::move(callback))
    , m_reason(std::move(reason))
{
}

void FailCallbackTask::run(GarrysMod::Lua::ILuaBase* LUA)
{
    if (!this->m_callback) {
        Logger::warn("Request failed without a fail hander: '%s'", m_reason.c_str());
        return;
    }

    m_callback->push(LUA);
    LUA->PushString(m_reason.c_str());
    LUA->Call(1, 0);
}
