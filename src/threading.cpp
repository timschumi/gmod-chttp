#include <GarrysMod/Lua/Interface.h>

#include <utility>
#include "chttp.h"
#include "threading.h"

using namespace GarrysMod;

LockableQueue<HTTPRequest*>& getRequestQueue() {
	static LockableQueue<HTTPRequest*> requests;
	return requests;
}

LockableQueue<FailedQueueData>& getFailQueue() {
	static LockableQueue<FailedQueueData> failed;
	return failed;
}

LockableQueue<SuccessQueueData>& getSuccessQueue() {
	static LockableQueue<SuccessQueueData> success;
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
		failed_data = getFailQueue().pop();
		runFailedHandler(LUA, failed_data.handler, failed_data.reason);
	}

	while (!getSuccessQueue().empty()) {
		success_data = getSuccessQueue().pop();
		runSuccessHandler(LUA, success_data.handler, success_data.response);
	}

	return 0;
}