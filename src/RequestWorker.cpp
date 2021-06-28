#include "RequestWorker.h"

RequestWorker &RequestWorker::the() {
	static RequestWorker instance;
	return instance;
}

void RequestWorker::run() {
	while (true) {
		auto request = requests().pop(true);

		if (request == nullptr)
			break;

		request->run();
	}
}

RequestWorker::RequestWorker() {
	_thread = std::thread([this] { run(); });
}
