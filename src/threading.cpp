#include <thread>

#include "chttp.h"
#include "threading.h"

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
