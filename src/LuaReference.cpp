#include "LuaReference.h"
#include "LuaReferenceFreeTask.h"
#include "RequestWorker.h"

LuaReference::LuaReference(GarrysMod::Lua::ILuaBase* LUA)
{
    m_reference = LUA->ReferenceCreate();
}

LuaReference::~LuaReference()
{
    if (!m_reference)
        return;

    RequestWorker::the().tasks().push(std::make_shared<LuaReferenceFreeTask>(m_reference));
}

void LuaReference::push(GarrysMod::Lua::ILuaBase* LUA) const
{
    LUA->ReferencePush(m_reference);
}
