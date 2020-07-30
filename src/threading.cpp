#include <GarrysMod/Lua/Interface.h>

#include <utility>
#include <thread>

#include "chttp.h"
#include "threading.h"

using namespace GarrysMod;

std::queue<HTTPRequest*>& getRequestQueue() {
	static std::queue<HTTPRequest*> requests;
	return requests;
}

std::queue<FailedQueueData>& getFailQueue() {
	static std::queue<FailedQueueData> failed;
	return failed;
}

std::queue<SuccessQueueData>& getSuccessQueue() {
	static std::queue<SuccessQueueData> success;
	return success;
}

bool scheduleRequest(HTTPRequest *request) {
	getRequestQueue().push(request);

	return startThread();
}

LUA_FUNCTION(threadingDoThink) {
	while (!getFailQueue().empty()) {
		FailedQueueData data = getFailQueue().front();
		getFailQueue().pop();
		runFailedHandler(LUA, data.handler, data.reason);
	}

	while (!getSuccessQueue().empty()) {
		SuccessQueueData data = getSuccessQueue().front();
		getSuccessQueue().pop();
		runSuccessHandler(LUA, data.handler, data.response);
		delete data.response;
	}

	return 0;
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
