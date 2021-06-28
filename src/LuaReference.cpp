#include "LuaReference.h"
#include "LuaReferenceFreeTask.h"
#include "RequestWorker.h"

LuaReference::LuaReference(GarrysMod::Lua::ILuaBase *LUA) {
	reference = LUA->ReferenceCreate();
}

LuaReference::~LuaReference() {
	if (!reference)
		return;

	RequestWorker::the().tasks().push(std::make_shared<LuaReferenceFreeTask>(reference));
}

void LuaReference::push(GarrysMod::Lua::ILuaBase *LUA) const {
	LUA->ReferencePush(reference);
}
