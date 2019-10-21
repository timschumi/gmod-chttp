#include <string>
#include <map>
#include <GarrysMod/Lua/Interface.h>

using namespace GarrysMod;

#define LOG(x) printMessage(LUA, x);

void mapToLuaTable(Lua::ILuaBase *LUA, std::map<std::string, std::string> map);
std::map<std::string, std::string> mapFromLuaTable(Lua::ILuaBase *LUA, int index);
void printMessage(Lua::ILuaBase *LUA, std::string message);
