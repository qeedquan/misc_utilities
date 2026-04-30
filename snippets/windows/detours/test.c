// https://github.com/Microsoft/Detours
// build detours as a library first and statically link it to program
#include <windows.h>
#include <detours.h>
#include <stdio.h>
#include <stdlib.h>

static void *(*RealMalloc)(size_t) = malloc;
static BOOL (WINAPI *RealIsDebuggerPresent)(void) = IsDebuggerPresent;
static void (WINAPI *RealOutputDebugString)(LPCTSTR) = OutputDebugString;
static LPVOID (WINAPI *RealVirtualAlloc)(LPVOID, SIZE_T, DWORD, DWORD) = VirtualAlloc;

void *
MyMalloc(size_t len)
{
	printf("%s(len = %zu)\n", __func__, len);
	return RealMalloc(len);
}

BOOL WINAPI
MyIsDebuggerPresent(void)
{
	printf("%s()\n", __func__);
	return 0;
}

static void WINAPI
MyOutputDebugString(LPCTSTR lpOutputString)
{
	printf("%s(%s)\n", __func__, lpOutputString);
}

static LPVOID WINAPI
MyVirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
	printf("%s(%p, %zu, %lx, %lx)\n", __func__, lpAddress, dwSize, flAllocationType, flProtect);
	return RealVirtualAlloc(lpAddress, dwSize, flAllocationType, flProtect);
}

int
main(void)
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID)RealMalloc, MyMalloc);
	DetourAttach(&(PVOID)RealIsDebuggerPresent, MyIsDebuggerPresent);
	DetourAttach(&(PVOID)RealOutputDebugString, MyOutputDebugString);
	DetourAttach(&(PVOID)RealVirtualAlloc, MyVirtualAlloc);
	DetourTransactionCommit();

	malloc(10);
	malloc(20);
	malloc(30);
	for (int i = 0; i < 10; i++)
		IsDebuggerPresent();

	MyOutputDebugString("Hello");
	VirtualAlloc(NULL, 100, 0, 0);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID)RealMalloc, MyMalloc);
	DetourDetach(&(PVOID)RealIsDebuggerPresent, MyIsDebuggerPresent);
	DetourDetach(&(PVOID)RealOutputDebugString, MyOutputDebugString);
	DetourDetach(&(PVOID)RealVirtualAlloc, MyVirtualAlloc);
	DetourTransactionCommit();

	return 0;
}