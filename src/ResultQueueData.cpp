#include "ResultQueueData.h"

#include "lua.h"

// TODO: Complain if SuccessHandler/FailedHandler has not been freed
ResultQueueData::~ResultQueueData() = default;

void ResultQueueData::freeHandlers(GarrysMod::Lua::ILuaBase *LUA) {
	if (this->SuccessHandler) {
		LUA->ReferenceFree(this->SuccessHandler);
		this->SuccessHandler = 0;
	}

	if (this->FailHandler) {
		LUA->ReferenceFree(this->FailHandler);
		this->FailHandler = 0;
	}
}

void ResultQueueData::removeHandlers() {
	this->SuccessHandler = 0;
	this->FailHandler = 0;
}

SuccessQueueData::SuccessQueueData(int SuccessHandler, int FailHandler) {
	this->SuccessHandler = SuccessHandler;
	this->FailHandler = FailHandler;
}

void SuccessQueueData::run(GarrysMod::Lua::ILuaBase *LUA) {
	if (!this->SuccessHandler) {
		return;
	}

	LUA->ReferencePush(this->SuccessHandler);
	LUA->PushNumber(this->code);
	LUA->PushString(this->body.c_str());
	mapToLuaTable(LUA, this->headers);
	LUA->Call(3, 0);
}

FailedQueueData::FailedQueueData(int SuccessHandler, int FailHandler, const std::string& reason) {
	this->SuccessHandler = SuccessHandler;
	this->FailHandler = FailHandler;
	this->reason = reason;
}

void FailedQueueData::run(GarrysMod::Lua::ILuaBase *LUA) {
	if (!this->FailHandler) {
		return;
	}

	LUA->ReferencePush(this->FailHandler);
	LUA->PushString(reason.c_str());
	LUA->Call(1, 0);
}
