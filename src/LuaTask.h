#pragma once

#include <string>
#include <map>

#include "GarrysMod/Lua/Interface.h"

class LuaTask {
public:
	virtual void run(GarrysMod::Lua::ILuaBase *LUA) = 0;
};

class RequestCallbackTask : public LuaTask {
protected:
	int SuccessHandler;
	int FailHandler;
public:
	virtual ~RequestCallbackTask();
};

// Data on the success queue
class SuccessCallbackTask : public RequestCallbackTask {
public:
	long code = 0;
	std::string body;
	std::map<std::string, std::string> headers;
public:
	SuccessCallbackTask(int SuccessHandler, int FailHandler);
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};

// Data on the success queue
class FailCallbackTask : public RequestCallbackTask {
private:
	std::string reason;
public:
	FailCallbackTask(int SuccessHandler, int FailHandler, const std::string& reason);
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};