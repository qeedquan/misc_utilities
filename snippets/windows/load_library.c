#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void
usage(void)
{
	fprintf(stderr, "usage: file\n");
	exit(2);
}

void
ErrorExit(LPTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
	    FORMAT_MESSAGE_ALLOCATE_BUFFER |
	        FORMAT_MESSAGE_FROM_SYSTEM |
	        FORMAT_MESSAGE_IGNORE_INSERTS,
	    NULL,
	    dw,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
	    (LPTSTR)&lpMsgBuf,
	    0, NULL);

	printf("%s failed with error %d: %s", lpszFunction, dw, (const char *)lpMsgBuf);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

void
load(const char *name)
{
	HMODULE module;

	module = LoadLibrary(name);
	if (!module)
		ErrorExit("LoadLibrary");
}

int
main(int argc, char *argv[])
{
	if (argc < 2)
		usage();

	load(argv[1]);
	return 0;
}
