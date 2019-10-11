#include "GarrysMod/Lua/Interface.h"

using namespace GarrysMod;

int Example(lua_State *state) {
	LUA->PushString("Hello from Example()!");

	return 1;
}

GMOD_MODULE_OPEN() {
	LUA->PushSpecial(Lua::SPECIAL_GLOB);
	LUA->PushString("Example");
	LUA->PushCFunction(Example);
	LUA->SetTable(-3);

	return 0;
}
