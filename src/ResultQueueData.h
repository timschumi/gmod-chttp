#include <string>

#include "GarrysMod/Lua/Interface.h"

#include "http.h"

class ResultQueueData {
protected:
	int SuccessHandler;
	int FailHandler;
public:
	virtual void run(GarrysMod::Lua::ILuaBase *LUA) = 0;
};

// Data on the success queue
class SuccessQueueData : ResultQueueData {
private:
	HTTPResponse *response;
public:
	SuccessQueueData(int SuccessHandler, int FailHandler, HTTPResponse *response);
	~SuccessQueueData();
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};

// Data on the success queue
class FailedQueueData : ResultQueueData {
private:
	std::string reason;
public:
	FailedQueueData(int SuccessHandler, int FailHandler, const std::string& reason);
	void run(GarrysMod::Lua::ILuaBase *LUA) override;
};