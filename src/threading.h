#include <queue>

#include "GarrysMod/Lua/Interface.h"

#include "http.h"
#include "ResultQueueData.h"

// Different queues for different things
std::queue<HTTPRequest*>& getRequestQueue();
std::queue<FailedQueueData*>& getFailQueue();
std::queue<SuccessQueueData*>& getSuccessQueue();

bool scheduleRequest(HTTPRequest *request);

bool startThread();
