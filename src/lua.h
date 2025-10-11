#pragma once

#include <map>
#include <string>

#include "GarrysMod/Lua/Interface.h"

extern int lua_ref_hook_Add;
extern int lua_ref_hook_Run;
extern int lua_ref_timer_Create;
extern int lua_ref_timer_Exists;

void map_to_lua_table(GarrysMod::Lua::ILuaBase* LUA, std::map<std::string, std::string> const& map);

void lua_table_to_map(GarrysMod::Lua::ILuaBase* LUA, int index, std::map<std::string, std::string>& map);

void register_zero_delay_timer(GarrysMod::Lua::ILuaBase* LUA, char const* identifier, GarrysMod::Lua::CFunc function);

void register_hook(GarrysMod::Lua::ILuaBase* LUA, char const* event, char const* identifier, GarrysMod::Lua::CFunc function);
