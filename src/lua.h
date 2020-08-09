#include <string>
#include <map>
#include <GarrysMod/Lua/Interface.h>

#define LOG(x) printMessage(LUA, x)

void mapToLuaTable(GarrysMod::Lua::ILuaBase *LUA, const std::map<std::string, std::string>& map);
std::map<std::string, std::string> mapFromLuaTable(GarrysMod::Lua::ILuaBase *LUA, int index);
void printMessage(GarrysMod::Lua::ILuaBase *LUA, const std::string& message);
