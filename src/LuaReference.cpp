#include "LuaReference.h"
#include "threading.h"
#include "LuaReferenceFreeTask.h"

LuaReference::LuaReference(GarrysMod::Lua::ILuaBase *LUA) {
	reference = LUA->ReferenceCreate();
}

LuaReference::~LuaReference() {
	if (!reference)
		return;

	getLuaTaskQueue().push(std::make_shared<LuaReferenceFreeTask>(reference));
}

void LuaReference::push(GarrysMod::Lua::ILuaBase *LUA) const {
	LUA->ReferencePush(reference);
}
