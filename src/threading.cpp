#include <thread>

#include "threading.h"

LockableQueue<HTTPRequest*>& getRequestQueue() {
	static LockableQueue<HTTPRequest*> requests;
	return requests;
}

LockableQueue<ResultQueueData*>& getResultQueue() {
	static LockableQueue<ResultQueueData*> failed;
	return failed;
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
		HTTPRequest *request = getRequestQueue().pop();
		request->run();
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
