#include <windows.h>

int APIENTRY
WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow)
{
	if (IsDebuggerPresent())
		MessageBox(NULL, "Debugger presence detected", "Debugger Test", MB_OK);
	else
		MessageBox(NULL, "No debugger presence detected", "Debugger Test", MB_OK);
	return 0;
}
