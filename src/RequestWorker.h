#pragma once

#include "GarrysMod/Lua/Interface.h"
#include "HTTPRequest.h"
#include "LockableQueue.h"
#include "LuaTask.h"
#include <memory>
#include <thread>

class RequestWorker {
public:
    LockableQueue<std::shared_ptr<HTTPRequest>>& requests() { return m_requests; }
    LockableQueue<std::shared_ptr<LuaTask>>& tasks() { return m_tasks; }

    void run_tasks(GarrysMod::Lua::ILuaBase* LUA);
    void stop();
    bool has_work();
    bool should_exit() const;

    static RequestWorker& the();

private:
    RequestWorker();

    void run();

    std::thread m_thread;

    bool m_processing_request = false;
    bool m_should_exit = false;

    LockableQueue<std::shared_ptr<HTTPRequest>> m_requests;
    LockableQueue<std::shared_ptr<LuaTask>> m_tasks;
};
