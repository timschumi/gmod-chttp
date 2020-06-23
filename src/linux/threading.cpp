#include <pthread.h>
#include "../chttp.h"
#include "../threading.h"

pthread_t thread;
bool thread_exists = false;

void* threadFunc(void *data) {
	thread_exists = true;
	while (!getRequestQueue().empty()) {
		HTTPRequest request = getRequestQueue().pop();
		processRequest(request);
	}
	thread_exists = false;
	pthread_exit(nullptr);
}

bool startThread() {
	if (thread_exists)
		return true;

	return pthread_create(&thread, nullptr, threadFunc, nullptr) == 0;
}