#include <pthread.h>
#include "../chttp.h"
#include "../threading.h"

pthread_t thread;
bool thread_exists = false;

void* threadFunc(void *data) {
	thread_exists = true;
	while (!requests.empty()) {
		HTTPRequest request = requests.pop();
		processRequest(request);
	}
	thread_exists = false;
	pthread_exit(NULL);
}

bool startThread() {
	if (thread_exists)
		return true;

	if (pthread_create(&thread, NULL, threadFunc, NULL))
		return false;

	return true;
}