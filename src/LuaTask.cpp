#include "LuaTask.h"

#include <utility>

#include "lua.h"
#include "Logger.h"

RequestCallbackTask::RequestCallbackTask(std::shared_ptr<LuaReference> callback)
		: callback(std::move(callback)) {
}

SuccessCallbackTask::SuccessCallbackTask(std::shared_ptr<LuaReference> callback)
		: RequestCallbackTask(std::move(callback)) {
}

void SuccessCallbackTask::run(GarrysMod::Lua::ILuaBase *LUA) {
	if (!this->callback) {
		return;
	}

	callback->push(LUA);
	LUA->PushNumber(this->code);
	LUA->PushString(this->body.c_str(), this->body.size());
	mapToLuaTable(LUA, this->headers);
	LUA->Call(3, 0);
}

FailCallbackTask::FailCallbackTask(std::shared_ptr<LuaReference> callback, std::string reason)
		: RequestCallbackTask(std::move(callback)), reason(std::move(reason)) {
}

void FailCallbackTask::run(GarrysMod::Lua::ILuaBase *LUA) {
	if (!this->callback) {
		Logger::warn("Request failed without a fail hander: '%s'", reason.c_str());
		return;
	}

	callback->push(LUA);
	LUA->PushString(reason.c_str());
	LUA->Call(1, 0);
}
