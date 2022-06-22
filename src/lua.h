#pragma once

#include <string>
#include <map>

#include "GarrysMod/Lua/Interface.h"

void mapToLuaTable(GarrysMod::Lua::ILuaBase *LUA, const std::map<std::string, std::string> &map);

void luaTableToMap(GarrysMod::Lua::ILuaBase *LUA, int index, std::map<std::string, std::string> &map);

void registerHook(GarrysMod::Lua::ILuaBase *LUA, char const* event, char const* identifier, GarrysMod::Lua::CFunc function);
