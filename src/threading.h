#include <GarrysMod/Lua/Interface.h>
#include "http.h"
#include "lockqueue.h"

// Data on the success queue
struct SuccessQueueData {
	int handler;
	HTTPResponse response;
};

// Data on the success queue
struct FailedQueueData {
	int handler;
	std::string reason;
};

// Different queues for different things
extern LockableQueue<HTTPRequest> requests;
extern LockableQueue<FailedQueueData> failed;
extern LockableQueue<SuccessQueueData> success;

// Implemented by LUA_FUNCTION(threadingDoThink)
int threadingDoThink(lua_State *L);

bool scheduleRequest(HTTPRequest request);

// The following functions should be implemented by
// the target-specific threading code
extern bool startThread();