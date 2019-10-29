#include <windows.h>
#include "../chttp.h"
#include "../threading.h"

HANDLE hThread;
DWORD dwThreadId;

DWORD WINAPI threadFunc(LPVOID data) {
	while (!requests.empty()) {
		HTTPRequest request = requests.pop();
		processRequest(request);
	}
	return 0;
}

bool startThread() {
	DWORD exitCode;

	if (GetExitCodeThread(hThread, &exitCode) && exitCode == STILL_ACTIVE)
		return true;

	hThread = CreateThread(NULL, 0, threadFunc, NULL, 0, &dwThreadId);

	if (hThread == NULL)
		return false;

	return true;
}