#include <string>
#include <map>
#include <GarrysMod/Lua/Interface.h>

void mapToLuaTable(GarrysMod::Lua::ILuaBase *LUA, std::map<std::string, std::string> map);
std::map<std::string, std::string> mapFromLuaTable(GarrysMod::Lua::ILuaBase *LUA, int index);
