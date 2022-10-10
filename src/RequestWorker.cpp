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
            true, [this] { return exited; }, [this] { processing_request = true; });

        if (request == nullptr)
            break;

        request->run();
        processing_request = false;
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
    _thread = std::thread([this] { run(); });
}

void RequestWorker::stop()
{
    // Mark the background thread for shutdown
    exited = true;

    // Cause the background thread to wake up
    requests().try_unblock();

    // Wait until the background thread exits by itself
    _thread.join();
}

bool RequestWorker::has_work()
{
    return !requests().empty() || !tasks().empty() || processing_request;
}

bool RequestWorker::should_exit() const
{
    return exited;
}
