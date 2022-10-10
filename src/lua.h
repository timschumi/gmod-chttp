#pragma once

#include <map>
#include <string>

#include "GarrysMod/Lua/Interface.h"

void mapToLuaTable(GarrysMod::Lua::ILuaBase* LUA, std::map<std::string, std::string> const& map);

void luaTableToMap(GarrysMod::Lua::ILuaBase* LUA, int index, std::map<std::string, std::string>& map);

void registerZeroDelayTimer(GarrysMod::Lua::ILuaBase* LUA, char const* identifier, GarrysMod::Lua::CFunc function);

void registerHook(GarrysMod::Lua::ILuaBase* LUA, char const* event, char const* identifier, GarrysMod::Lua::CFunc function);
