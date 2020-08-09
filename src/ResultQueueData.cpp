#include "ResultQueueData.h"

#include "lua.h"

// TODO: Complain if SuccessHandler/FailedHandler has not been freed
ResultQueueData::~ResultQueueData() = default;

SuccessQueueData::SuccessQueueData(int SuccessHandler, int FailHandler, HTTPResponse *response) {
	this->SuccessHandler = SuccessHandler;
	this->FailHandler = FailHandler;
	this->response = response;
}

SuccessQueueData::~SuccessQueueData() {
	delete this->response;
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
	LUA->PushNumber((double) response->code);
	LUA->PushString(response->body.c_str());
	mapToLuaTable(LUA, response->headers);

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
