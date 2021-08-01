#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class LockableQueue {
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable cond;

public:
	void push(T element);

	template<typename P>
	T pop(bool block, P pred);
	T pop(bool block);
	T pop();

	bool empty();

	void try_unblock();
};

template<class T>
void LockableQueue<T>::push(T element) {
	std::unique_lock<std::mutex> lock(mutex);

	queue.push(element);

	// Unlock manually so that cond doesn't immediately wait again.
	lock.unlock();

	this->cond.notify_one();
}

template<class T>
template<typename P>
T LockableQueue<T>::pop(bool block, P pred) {
	std::unique_lock<std::mutex> lock(mutex);

	if (block) {
		this->cond.wait(lock, [this, pred] { return !queue.empty() || pred(); });
	}

	if (queue.empty())
		return nullptr;

	T element = queue.front();
	queue.pop();

	return element;
}

template<class T>
T LockableQueue<T>::pop(bool block) {
	return pop(block, [] { return false; });
}

template<class T>
T LockableQueue<T>::pop() {
	return pop(false);
}

template<class T>
bool LockableQueue<T>::empty() {
	std::unique_lock<std::mutex> lock(mutex);

	return queue.empty();
}

template<class T>
void LockableQueue<T>::try_unblock() {
	cond.notify_one();
}
