#include "GarrysMod/Lua/Interface.h"
#include "http.h"

#define CHTTP_VERSION "1.2"

bool processRequest(HTTPRequest request);
void runFailedHandler(GarrysMod::Lua::ILuaBase *LUA, int handler, std::string reason);
void runSuccessHandler(GarrysMod::Lua::ILuaBase *LUA, int handler, HTTPResponse response);
