#include "threading.h"

LockableQueue<std::shared_ptr<HTTPRequest>>& getRequestQueue() {
	static LockableQueue<std::shared_ptr<HTTPRequest>> requests;
	return requests;
}

LockableQueue<std::shared_ptr<ResultQueueData>>& getResultQueue() {
	static LockableQueue<std::shared_ptr<ResultQueueData>> failed;
	return failed;
}

std::queue<int>& getReferenceFreeQueue() {
	static std::queue<int> queue;
	return queue;
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
