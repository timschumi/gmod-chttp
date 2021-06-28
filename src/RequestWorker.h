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

	void join() { return _thread.join(); };

	static RequestWorker &the();
private:
	RequestWorker();

	void run();

	std::thread _thread;

	LockableQueue<std::shared_ptr<HTTPRequest>> _requests;
	LockableQueue<std::shared_ptr<LuaTask>> _tasks;
};

