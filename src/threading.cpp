#include "threading.h"

LockableQueue<std::shared_ptr<HTTPRequest>>& getRequestQueue() {
	static LockableQueue<std::shared_ptr<HTTPRequest>> requests;
	return requests;
}

LockableQueue<std::shared_ptr<ResultQueueData>>& getResultQueue() {
	static LockableQueue<std::shared_ptr<ResultQueueData>> failed;
	return failed;
}

void threadFunc() {
	while (true) {
		auto request = getRequestQueue().pop(true);

		if (request == nullptr)
			break;

		request->run();
	}
}

std::thread& getBackgroundThread() {
	static std::thread thread(threadFunc);
	return thread;
}
