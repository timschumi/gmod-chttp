#include <GarrysMod/Lua/Interface.h>

#include <utility>
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
	FailedQueueData failed_data;
	SuccessQueueData success_data;
	while (!getFailQueue().empty()) {
		failed_data = getFailQueue().front();
		getFailQueue().pop();
		runFailedHandler(LUA, failed_data.handler, failed_data.reason);
	}

	while (!getSuccessQueue().empty()) {
		success_data = getSuccessQueue().front();
		getSuccessQueue().pop();
		runSuccessHandler(LUA, success_data.handler, success_data.response);
	}

	return 0;
}
