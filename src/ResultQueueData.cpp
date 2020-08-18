#include "ResultQueueData.h"

#include "lua.h"

// TODO: Complain if SuccessHandler/FailedHandler has not been freed
ResultQueueData::~ResultQueueData() = default;

SuccessQueueData::SuccessQueueData(int SuccessHandler, int FailHandler) {
	this->SuccessHandler = SuccessHandler;
	this->FailHandler = FailHandler;
}

void SuccessQueueData::run(GarrysMod::Lua::ILuaBase *LUA) {
	if(this->FailHandler)
		LUA->ReferenceFree(this->FailHandler);

	if(!this->SuccessHandler) {
		return;
	}

	// Push success handler to stack and free our ref
	LUA->ReferencePush(this->SuccessHandler);
	LUA->ReferenceFree(this->SuccessHandler);

	// Push the arguments
	LUA->PushNumber(this->code);
	LUA->PushString(this->body.c_str());
	mapToLuaTable(LUA, this->headers);

	// Call the success handler with three arguments
	LUA->Call(3, 0);
}

FailedQueueData::FailedQueueData(int SuccessHandler, int FailHandler, const std::string& reason) {
	this->SuccessHandler = SuccessHandler;
	this->FailHandler = FailHandler;
	this->reason = reason;
}

void FailedQueueData::run(GarrysMod::Lua::ILuaBase *LUA) {
	if(this->SuccessHandler)
		LUA->ReferenceFree(this->SuccessHandler);

	if(!this->FailHandler) {
		return;
	}

	// Push fail handler to stack and free our ref
	LUA->ReferencePush(this->FailHandler);
	LUA->ReferenceFree(this->FailHandler);

	// Push the argument
	LUA->PushString(reason.c_str());

	// Call the fail handler with one argument
	LUA->Call(1, 0);
}
