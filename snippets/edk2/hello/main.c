// https://wiki.osdev.org/GNU-EFI
#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	int i;

	InitializeLib(ImageHandle, SystemTable);
	for (i = 0; i < 10; i++)
		Print(L"Hello, %d\n", i);
	return EFI_SUCCESS;
}
