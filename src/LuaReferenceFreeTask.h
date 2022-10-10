#pragma once

#include "LuaTask.h"

class LuaReferenceFreeTask : public LuaTask {
private:
    int m_reference;

public:
    explicit LuaReferenceFreeTask(int ref);

    void run(GarrysMod::Lua::ILuaBase* LUA) override;
};
