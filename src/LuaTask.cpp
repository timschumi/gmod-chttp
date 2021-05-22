#include "LuaTask.h"

#include "lua.h"
#include "Logger.h"
#include "threading.h"
#include "LuaReferenceFreeTask.h"

RequestCallbackTask::~RequestCallbackTask() {
	if (this->SuccessHandler) {
		getLuaTaskQueue().push(std::make_shared<LuaReferenceFreeTask>(this->SuccessHandler));
		this->SuccessHandler = 0;
	}

	if (this->FailHandler) {
		getLuaTaskQueue().push(std::make_shared<LuaReferenceFreeTask>(this->FailHandler));
		this->FailHandler = 0;
	}
}

SuccessCallbackTask::SuccessCallbackTask(int SuccessHandler, int FailHandler) {
	this->SuccessHandler = SuccessHandler;
	this->FailHandler = FailHandler;
}

void SuccessCallbackTask::run(GarrysMod::Lua::ILuaBase *LUA) {
	if (!this->SuccessHandler) {
		return;
	}

	LUA->ReferencePush(this->SuccessHandler);
	LUA->PushNumber(this->code);
	LUA->PushString(this->body.c_str());
	mapToLuaTable(LUA, this->headers);
	LUA->Call(3, 0);
}

FailCallbackTask::FailCallbackTask(int SuccessHandler, int FailHandler, const std::string& reason) {
	this->SuccessHandler = SuccessHandler;
	this->FailHandler = FailHandler;
	this->reason = reason;
}

void FailCallbackTask::run(GarrysMod::Lua::ILuaBase *LUA) {
	if (!this->FailHandler) {
		Logger::warn("Request failed without a fail hander: '%s'", reason.c_str());
		return;
	}

	LUA->ReferencePush(this->FailHandler);
	LUA->PushString(reason.c_str());
	LUA->Call(1, 0);
}
