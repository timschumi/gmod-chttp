#pragma once

#include "GarrysMod/Lua/Interface.h"
#include "HTTPRequest.h"
#include "LockableQueue.h"
#include "LuaTask.h"
#include <memory>
#include <thread>

class RequestWorker {
public:
	LockableQueue<std::shared_ptr<HTTPRequest>> &requests() { return _requests; };
	LockableQueue<std::shared_ptr<LuaTask>> &tasks() { return _tasks; };

	void run_tasks(GarrysMod::Lua::ILuaBase *LUA);
	void stop();
	bool has_work();

	static RequestWorker &the();
private:
	RequestWorker();

	void run();

	std::thread _thread;

	bool processing_request = false;
	bool exited = false;

	LockableQueue<std::shared_ptr<HTTPRequest>> _requests;
	LockableQueue<std::shared_ptr<LuaTask>> _tasks;
};

