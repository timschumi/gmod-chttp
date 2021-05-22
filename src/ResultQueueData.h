#pragma once

#include <string>
#include <map>

#include "GarrysMod/Lua/Interface.h"

class ResultQueueData {
protected:
	int SuccessHandler;
	int FailHandler;
public:
	virtual ~ResultQueueData();
	virtual void run(GarrysMod::Lua::ILuaBase *LUA) = 0;
	void freeHandlers(GarrysMod::Lua::ILuaBase *LUA);
	void removeHandlers();
};

// Data on the success queue
class SuccessQueueData : public ResultQueueData {
public:
	long code = 0;
	std::string body;
	std::map<std::string, std::string> headers;
public:
	SuccessQueueData(int SuccessHandler, int FailHandler);
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};

// Data on the success queue
class FailedQueueData : public ResultQueueData {
private:
	std::string reason;
public:
	FailedQueueData(int SuccessHandler, int FailHandler, const std::string& reason);
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};