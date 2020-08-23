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
	size_t size();
	bool empty();
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

	T element = queue.front();
	queue.pop();

	lock.unlock();
	return element;
}

template <class T>
size_t LockableQueue<T>::size() {
	std::lock_guard<std::mutex> lock(mutex);
	return queue.size();
}

template <class T>
bool LockableQueue<T>::empty() {
	return this->size() == 0;
}
