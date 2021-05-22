#include "LuaReferenceFreeTask.h"

LuaReferenceFreeTask::LuaReferenceFreeTask(int ref) : reference(ref) {
}

void LuaReferenceFreeTask::run(GarrysMod::Lua::ILuaBase *LUA) {
	if (!reference)
		return;

	LUA->ReferenceFree(reference);
	reference = 0;
}
