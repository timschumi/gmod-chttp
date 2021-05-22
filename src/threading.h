#pragma once

#include <thread>

#include "GarrysMod/Lua/Interface.h"

#include "HTTPRequest.h"
#include "ResultQueueData.h"
#include "LockableQueue.h"

// Different queues for different things
LockableQueue<std::shared_ptr<HTTPRequest>>& getRequestQueue();
LockableQueue<std::shared_ptr<ResultQueueData>>& getResultQueue();
std::queue<int>& getReferenceFreeQueue();

std::thread& getBackgroundThread();
