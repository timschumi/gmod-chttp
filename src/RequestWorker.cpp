#include "RequestWorker.h"

RequestWorker& RequestWorker::the()
{
    static RequestWorker instance;
    return instance;
}

void RequestWorker::run()
{
    while (true) {
        auto request = requests().pop(
            true, [this] { return m_should_exit; }, [this] { m_processing_request = true; });

        if (request == nullptr)
            break;

        request->run();
        m_processing_request = false;
    }
}

void RequestWorker::run_tasks(GarrysMod::Lua::ILuaBase* LUA)
{
    while (true) {
        auto data = tasks().pop();

        if (data == nullptr)
            break;

        data->run(LUA);
    }
}

RequestWorker::RequestWorker()
{
    m_thread = std::thread([this] { run(); });
}

void RequestWorker::stop()
{
    // Mark the background thread for shutdown
    m_should_exit = true;

    // Cause the background thread to wake up
    requests().try_unblock();

    // Wait until the background thread exits by itself
    m_thread.join();
}

bool RequestWorker::has_work()
{
    return !requests().empty() || !tasks().empty() || m_processing_request;
}

bool RequestWorker::should_exit() const
{
    return m_should_exit;
}
