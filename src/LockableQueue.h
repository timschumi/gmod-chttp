#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class LockableQueue {
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable cond;

public:
	void push(T element);
	T pop(bool block = false);
};

template <class T>
void LockableQueue<T>::push(T element) {
	std::unique_lock<std::mutex> lock(mutex);

	queue.push(element);
	lock.unlock();

	this->cond.notify_one();
}

template <class T>
T LockableQueue<T>::pop(bool block) {
	std::unique_lock<std::mutex> lock(mutex);

	if (block) {
		this->cond.wait(lock, [this] { return !queue.empty(); });
	}

	if (queue.empty())
		return nullptr;

	T element = queue.front();
	queue.pop();

	lock.unlock();
	return element;
}
