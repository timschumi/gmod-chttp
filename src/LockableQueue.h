#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template<class T>
class LockableQueue {
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cond;

public:
    void push(T element);

    template<typename P, typename P2>
    T pop(bool block, P pred, P2 before_pop);
    template<typename P>
    T pop(bool block, P pred);
    T pop(bool block);
    T pop();

    bool empty();

    void try_unblock();
};

template<class T>
void LockableQueue<T>::push(T element)
{
    std::unique_lock<std::mutex> lock(mutex);

    queue.push(element);

    // Unlock manually so that cond doesn't immediately wait again.
    lock.unlock();

    this->cond.notify_one();
}

template<class T>
template<typename P, typename P2>
T LockableQueue<T>::pop(bool block, P pred, P2 before_pop)
{
    std::unique_lock<std::mutex> lock(mutex);

    if (block) {
        this->cond.wait(lock, [this, pred] { return !queue.empty() || pred(); });
    }

    if (queue.empty())
        return nullptr;

    T element = queue.front();

    before_pop();

    queue.pop();

    return element;
}

template<class T>
template<typename P>
T LockableQueue<T>::pop(bool block, P pred)
{
    return pop(block, pred, [] {});
}

template<class T>
T LockableQueue<T>::pop(bool block)
{
    return pop(block, [] { return false; });
}

template<class T>
T LockableQueue<T>::pop()
{
    return pop(false);
}

template<class T>
bool LockableQueue<T>::empty()
{
    std::unique_lock<std::mutex> lock(mutex);

    return queue.empty();
}

template<class T>
void LockableQueue<T>::try_unblock()
{
    cond.notify_one();
}
