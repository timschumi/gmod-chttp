#pragma once

#include "HTTPRequest.h"
#include "LockableQueue.h"
#include "LuaTask.h"
#include <memory>
#include <thread>

class RequestWorker {
public:
	LockableQueue<std::shared_ptr<HTTPRequest>> &requests() { return _requests; };
	LockableQueue<std::shared_ptr<LuaTask>> &tasks() { return _tasks; };

	void stop();

	static RequestWorker &the();
private:
	RequestWorker();

	void run();

	std::thread _thread;

	bool exited = false;

	LockableQueue<std::shared_ptr<HTTPRequest>> _requests;
	LockableQueue<std::shared_ptr<LuaTask>> _tasks;
};

