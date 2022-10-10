#include "LuaReferenceFreeTask.h"

LuaReferenceFreeTask::LuaReferenceFreeTask(int ref)
    : m_reference(ref)
{
}

void LuaReferenceFreeTask::run(GarrysMod::Lua::ILuaBase* LUA)
{
    if (!m_reference)
        return;

    LUA->ReferenceFree(m_reference);
    m_reference = 0;
}
