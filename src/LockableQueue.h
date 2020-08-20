#include <queue>
#include <mutex>

template <class T>
class LockableQueue {
	std::queue<T> queue;
	std::mutex mutex;

public:
	void push(T element);
	T pop();
	size_t size();
	bool empty();
};

template <class T>
void LockableQueue<T>::push(T element) {
	std::lock_guard<std::mutex> lock(mutex);
	queue.push(element);
}

template <class T>
T LockableQueue<T>::pop() {
	T element;
	std::lock_guard<std::mutex> lock(mutex);

	element = queue.front();
	queue.pop();

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
