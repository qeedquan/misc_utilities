#include <efi.h>
#include <efilib.h>

void EFIAPI
DisplayGlobals(void)
{
	Print(L"ST %lx\n", (unsigned long)ST);
	Print(L"BS %lx\n", (unsigned long)BS);
}

EFI_STATUS EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	UINTN i;

	InitializeLib(ImageHandle, SystemTable);
	Print(L"Starting Loop\n");
	DisplayGlobals();

	for (i = 0;; i++) {
		Print(L"%u\n", i);
		uefi_call_wrapper(BS->Stall, 1, 1000000UL);
	}

	return EFI_SUCCESS;
}
