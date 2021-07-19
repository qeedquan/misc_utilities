#include <stdio.h>
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <winnt.h>

typedef NTSTATUS(NTAPI *NtQuerySystemProcessFunc)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

PPEB
FindPEB(void)
{
	NtQuerySystemProcessFunc Query;
	PROCESS_BASIC_INFORMATION Info;
	HANDLE Process;
	ULONG Length;
	NTSTATUS Status;

	Query = (NtQuerySystemProcessFunc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
	if (!Query)
		return NULL;
	Process = GetCurrentProcess();
	Length = sizeof(Info);
	Status = Query(Process, ProcessBasicInformation, &Info, sizeof(Info), &Length);
	if (Status != STATUS_SUCCESS)
		return NULL;
	return Info.PebBaseAddress;
}

void
PrintPEB(PPEB Peb)
{
	printf("PEB Base Address: %p\n", Peb);
	if (!Peb)
		return;
	printf("BeingDebugged : %d\n", Peb->BeingDebugged);
	printf("SessionId     : %lx\n", (unsigned long)Peb->SessionId);
}

int
main(void)
{
	struct _TEB *Teb;
	PPEB Peb;

	Peb = FindPEB();
	PrintPEB(Peb);
	if (Peb) {
		Peb->BeingDebugged = 1;
		printf("%d\n", IsDebuggerPresent());
		Peb->BeingDebugged = 0;
		printf("%d\n", IsDebuggerPresent());
	}

	Teb = NtCurrentTeb();
#if defined(__WINE__)
	printf("%p %p\n", Peb, Teb->Peb);
#else
	printf("%p %p\n", Peb, Teb->ProcessEnvironmentBlock);
#endif

	return 0;
}
