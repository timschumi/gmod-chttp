#include <thread>

#include "lua.h"
#include "chttp.h"
#include "threading.h"

std::queue<HTTPRequest*>& getRequestQueue() {
	static std::queue<HTTPRequest*> requests;
	return requests;
}

std::queue<FailedQueueData*>& getFailQueue() {
	static std::queue<FailedQueueData*> failed;
	return failed;
}

std::queue<SuccessQueueData*>& getSuccessQueue() {
	static std::queue<SuccessQueueData*> success;
	return success;
}

bool scheduleRequest(HTTPRequest *request) {
	getRequestQueue().push(request);

	return startThread();
}

// Functions for the different queue entries
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


// Actual threads
bool thread_exists = false;

void threadFunc() {
	thread_exists = true;
	while (!getRequestQueue().empty()) {
		HTTPRequest *request = getRequestQueue().front();
		getRequestQueue().pop();
		processRequest(request);
		delete request;
	}
	thread_exists = false;
}

bool startThread() {
	// Is the thread still running?
	if (thread_exists)
		return true;

	std::thread thread(threadFunc);
	thread.detach();

	// Should be fine, unfortunately C++ threads don't deliver much insight.
	return true;
}
