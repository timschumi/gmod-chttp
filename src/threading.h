#include <queue>

#include "GarrysMod/Lua/Interface.h"

#include "http.h"
#include "ResultQueueData.h"

// Different queues for different things
std::queue<HTTPRequest*>& getRequestQueue();
std::queue<ResultQueueData*>& getResultQueue();

bool scheduleRequest(HTTPRequest *request);

bool startThread();
