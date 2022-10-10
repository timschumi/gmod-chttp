#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template<class T>
class LockableQueue {
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;

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
    std::unique_lock<std::mutex> lock(m_mutex);

    m_queue.push(element);

    // Unlock manually so that m_cond doesn't immediately wait again.
    lock.unlock();

    this->m_cond.notify_one();
}

template<class T>
template<typename P, typename P2>
T LockableQueue<T>::pop(bool block, P pred, P2 before_pop)
{
    std::unique_lock<std::mutex> lock(m_mutex);

    if (block) {
        this->m_cond.wait(lock, [this, pred] { return !m_queue.empty() || pred(); });
    }

    if (m_queue.empty())
        return nullptr;

    T element = m_queue.front();

    before_pop();

    m_queue.pop();

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
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_queue.empty();
}

template<class T>
void LockableQueue<T>::try_unblock()
{
    m_cond.notify_one();
}
