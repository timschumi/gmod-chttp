#include <GarrysMod/Lua/Interface.h>
#include "chttp.h"
#include "threading.h"

using namespace GarrysMod;

LockableQueue<HTTPRequest> requests;
LockableQueue<FailedQueueData> failed;
LockableQueue<SuccessQueueData> success;

bool scheduleRequest(HTTPRequest request) {
	requests.push(request);

	return startThread();
}

LUA_FUNCTION(threadingDoThink) {
	FailedQueueData failed_data;
	SuccessQueueData success_data;
	while (!failed.empty()) {
		failed_data = failed.pop();
		runFailedHandler(LUA, failed_data.handler, failed_data.reason);
	}

	while (!success.empty()) {
		success_data = success.pop();
		runSuccessHandler(LUA, success_data.handler, success_data.response);
	}

	return 0;
}