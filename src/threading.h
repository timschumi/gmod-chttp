#include <queue>

#include "GarrysMod/Lua/Interface.h"

#include "http.h"

// Data on the success queue
class SuccessQueueData {
private:
	int SuccessHandler;
	int FailHandler;
	HTTPResponse *response;
public:
	SuccessQueueData(int SuccessHandler, int FailHandler, HTTPResponse *response);
	~SuccessQueueData();
	void run(GarrysMod::Lua::ILuaBase *LUA);
};

// Data on the success queue
class FailedQueueData {
private:
	int SuccessHandler;
	int FailHandler;
	std::string reason;
public:
	FailedQueueData(int SuccessHandler, int FailHandler, const std::string& reason);
	void run(GarrysMod::Lua::ILuaBase *LUA);
};

// Different queues for different things
std::queue<HTTPRequest*>& getRequestQueue();
std::queue<FailedQueueData*>& getFailQueue();
std::queue<SuccessQueueData*>& getSuccessQueue();

bool scheduleRequest(HTTPRequest *request);

bool startThread();
