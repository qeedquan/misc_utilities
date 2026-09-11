#include <stdio.h>
#include <windows.h>

int
main(void)
{
	printf("REGSAM                   %zu\n", sizeof(REGSAM));
	printf("NTSTATUS                 %zu\n", sizeof(NTSTATUS));
	printf("LSTATUS                  %zu\n", sizeof(LSTATUS));
	printf("PHKEY                    %zu\n", sizeof(PHKEY));
	printf("HKEY                     %zu\n", sizeof(HKEY));
	printf("BYTE                     %zu\n", sizeof(BYTE));
	printf("WORD                     %zu\n", sizeof(WORD));
	printf("PWORD                    %zu\n", sizeof(PWORD));
	printf("DWORD                    %zu\n", sizeof(DWORD));
	printf("DWORD64                  %zu\n", sizeof(DWORD64));
	printf("SIZE_T                   %zu\n", sizeof(SIZE_T));
	printf("PDWORD                   %zu\n", sizeof(PDWORD));
	printf("PVOID                    %zu\n", sizeof(PVOID));
	printf("LPVOID                   %zu\n", sizeof(LPVOID));
	printf("HINSTANCE                %zu\n", sizeof(HINSTANCE));
	printf("HANDLE                   %zu\n", sizeof(HANDLE));
	printf("HWND                     %zu\n", sizeof(HWND));
	printf("PSTR                     %zu\n", sizeof(PSTR));
	printf("LPSTR                    %zu\n", sizeof(LPSTR));
	printf("LPCSTR                   %zu\n", sizeof(LPCSTR));
	printf("CHAR                     %zu\n", sizeof(CHAR));
	printf("INT                      %zu\n", sizeof(INT));
	printf("UINT                     %zu\n", sizeof(UINT));
	printf("SYSTEMTIME               %zu\n", sizeof(SYSTEMTIME));
	printf("PROC                     %zu\n", sizeof(PROC));
	printf("RECT                     %zu\n", sizeof(RECT));
	printf("CRITICAL_SECTION         %zu\n", sizeof(CRITICAL_SECTION));
	printf("SECURITY_ATTRIBUTES      %zu\n", sizeof(SECURITY_ATTRIBUTES));
	printf("MEMORY_BASIC_INFORMATION %zu\n", sizeof(MEMORY_BASIC_INFORMATION));
	return 0;
}
