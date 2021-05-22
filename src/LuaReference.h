#pragma once


#include <GarrysMod/Lua/Interface.h>

class LuaReference {
private:
	int reference;

public:
	explicit LuaReference(GarrysMod::Lua::ILuaBase *LUA);

	~LuaReference();

	void push(GarrysMod::Lua::ILuaBase *LUA) const;
};
