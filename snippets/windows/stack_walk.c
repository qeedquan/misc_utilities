// cc -o stack_walk stack_walk.c -limagehlp -ldbghelp -lBCrypt -Wall -Wextra
#include <stdio.h>
#include <windows.h>
#include <dbghelp.h>
#include <time.h>

#define ArraySize(x) (sizeof(x) / sizeof(x[0]))

CRITICAL_SECTION critSec;

void
PrintStack(void)
{
	unsigned int i;
	void *stack[1024];
	unsigned short frames;
	SYMBOL_INFO *symbol;
	HANDLE process;

	EnterCriticalSection(&critSec);
	process = GetCurrentProcess();

	SymInitialize(process, NULL, TRUE);

	frames = CaptureStackBackTrace(0, ArraySize(stack), stack, NULL);
	symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (i = 0; i < frames; i++) {
		SymFromAddr(process, (uintptr_t)(stack[i]), 0, symbol);
		printf("%-2i (%p): %s - 0x%0lX\n", frames - i - 1, stack[i], symbol->Name, (unsigned long)symbol->Address);
	}

	free(symbol);
	LeaveCriticalSection(&critSec);
}

void
Bar(int n)
{
	if (n <= 0) {
		PrintStack();
		return;
	}
	Bar(n - 1);
}

void
Foo(void)
{
	Bar(32);
}

DWORD WINAPI
Runner(LPVOID lpParam)
{
	UCHAR Buffer[sizeof(UINT)];
	UINT Num;

	EnterCriticalSection(&critSec);
	BCryptGenRandom(NULL, Buffer, sizeof(Buffer), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
	LeaveCriticalSection(&critSec);
	Num = Buffer[0] | Buffer[1] << 8 | Buffer[2] << 16 | Buffer[3] << 24;
	Num = Num % 100;

	printf("Thread %#lx %d\n", (unsigned long)GetCurrentThreadId(), Num);
	Bar(Num);

	(void)lpParam;
	return 0;
}

int
main(int argc, char *argv[])
{
	HANDLE Thread[32];
	size_t i;

	InitializeCriticalSection(&critSec);

	Foo();
	PrintStack();

	srand(time(NULL));
	for (i = 0; i < _countof(Thread); i++)
		Thread[i] = CreateThread(NULL, 0, Runner, NULL, 0, NULL);

	WaitForMultipleObjects(_countof(Thread), Thread, TRUE, INFINITE);
	(void)argc;
	(void)argv;
	return 0;
}