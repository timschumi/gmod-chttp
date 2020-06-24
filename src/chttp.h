#include "GarrysMod/Lua/Interface.h"
#include "http.h"

#define CHTTP_VERSION "1.3.1"

bool processRequest(const HTTPRequest& request);
void runFailedHandler(GarrysMod::Lua::ILuaBase *LUA, int handler, const std::string& reason);
void runSuccessHandler(GarrysMod::Lua::ILuaBase *LUA, int handler, const HTTPResponse& response);
