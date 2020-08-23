#include "threading.h"

LockableQueue<HTTPRequest*>& getRequestQueue() {
	static LockableQueue<HTTPRequest*> requests;
	return requests;
}

LockableQueue<ResultQueueData*>& getResultQueue() {
	static LockableQueue<ResultQueueData*> failed;
	return failed;
}

void threadFunc() {
	while (true) {
		HTTPRequest *request = getRequestQueue().pop(true);

		if (request == nullptr)
			break;

		request->run();
		delete request;
	}
}

std::thread& getBackgroundThread() {
	static std::thread thread(threadFunc);
	return thread;
}
