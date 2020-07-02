#include <windows.h>
#include "../chttp.h"
#include "../threading.h"

HANDLE hThread;
DWORD dwThreadId;

DWORD WINAPI threadFunc(LPVOID data) {
	while (!getRequestQueue().empty()) {
		HTTPRequest *request = getRequestQueue().front();
		getRequestQueue().pop();
		processRequest(request);
		delete request;
	}
	return 0;
}

bool startThread() {
	DWORD exitCode;

	if (GetExitCodeThread(hThread, &exitCode) && exitCode == STILL_ACTIVE)
		return true;

	// Free old thread
	if (hThread && !CloseHandle(hThread))
		return false;

	hThread = CreateThread(nullptr, 0, threadFunc, nullptr, 0, &dwThreadId);

	return hThread != nullptr;
}
