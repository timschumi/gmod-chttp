#pragma once

#include <thread>

#include "GarrysMod/Lua/Interface.h"

#include "HTTPRequest.h"
#include "LuaTask.h"
#include "LockableQueue.h"

// Different queues for different things
LockableQueue<std::shared_ptr<HTTPRequest>> &getRequestQueue();

LockableQueue<std::shared_ptr<LuaTask>> &getLuaTaskQueue();

std::thread &getBackgroundThread();
