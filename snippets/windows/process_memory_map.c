#include <Windows.h>
#include <psapi.h>
#include <stdio.h>

void
FormatMbiString(MEMORY_BASIC_INFORMATION *Mbi, char *Buffer)
{
	int n;

	n = sprintf(Buffer, "%016lx-%016lx %-8x ", (unsigned long)(uintptr_t)Mbi->BaseAddress, (unsigned long)((uintptr_t)Mbi->BaseAddress + Mbi->RegionSize), (UINT)Mbi->RegionSize);
	switch (Mbi->Type) {
	case MEM_IMAGE:
		n += sprintf(Buffer + n, "IMAGE   ");
		break;
	case MEM_MAPPED:
		n += sprintf(Buffer + n, "MAPPED  ");
		break;
	case MEM_PRIVATE:
		n += sprintf(Buffer + n, "PRIVATE ");
		break;
	default:
		n += sprintf(Buffer + n, "UNMAPPED");
		break;
	}

	n += sprintf(Buffer + n, " ");
	switch (Mbi->Type) {
	case MEM_COMMIT:
		n += sprintf(Buffer + n, "COMMIT");
		break;
	case MEM_FREE:
		n += sprintf(Buffer + n, "FREE");
		break;
	case MEM_RESERVE:
		n += sprintf(Buffer + n, "RESERVE");
		break;
	default:
		n += sprintf(Buffer + n, "N/A");
		break;
	}

	n += sprintf(Buffer + n, " (%04lx) ", (unsigned long)Mbi->Protect);
	if (Mbi->Protect & 0x80)
		n += sprintf(Buffer + n, "WC");
	if (Mbi->Protect & 0x40)
		n += sprintf(Buffer + n, "RWX");
	if (Mbi->Protect & 0x10)
		n += sprintf(Buffer + n, "R");
	if (Mbi->Protect & 0x20)
		n += sprintf(Buffer + n, "X");
	if (Mbi->Protect & 0x02)
		n += sprintf(Buffer + n, "R");
	if (Mbi->Protect & 0x04)
		n += sprintf(Buffer + n, "RW");
	if (Mbi->Protect & 0x08)
		n += sprintf(Buffer + n, "WC");
	if (Mbi->Protect < 0x2)
		n += sprintf(Buffer + n, "-");
}

void
DumpProcessMemory(void)
{
	char Buffer[1024], Filename[MAX_PATH];
	MEMORY_BASIC_INFORMATION Mbi;
	HANDLE Handle;
	LPVOID Addr;

	Handle = GetCurrentProcess();
	Addr = NULL;
	for (;;) {
		if (!VirtualQueryEx(Handle, Addr, &Mbi, sizeof(Mbi)))
			break;
		if (Mbi.RegionSize > ((uintptr_t)-1) - ((uintptr_t)Mbi.BaseAddress))
			break;

		Filename[0] = '\0';
		K32GetMappedFileNameA(Handle, Addr, Filename, sizeof(Filename));

		FormatMbiString(&Mbi, Buffer);
		printf("%s\t%s\n", Buffer, Filename);

		Addr = Mbi.BaseAddress + Mbi.RegionSize;
		if (Addr == NULL)
			break;
	}
}

int
main(int argc, char *argv[])
{
	DumpProcessMemory();
	return 0;
}
