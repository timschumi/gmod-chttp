#include <deque>
#include <mutex>

template <class T>
class LockableQueue {
	std::deque<T> queue;
	std::mutex mutex;

public:
	void push(T element);
	T pop();
};