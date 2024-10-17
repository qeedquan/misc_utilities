#include <efi.h>
#include <efilib.h>
#include <eficon.h>
#include <efiprot.h>
#include <efigpt.h>

EFI_STATUS EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_INPUT_KEY key;
	EFI_STATUS r;

	InitializeLib(ImageHandle, SystemTable);

	uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);
	uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, EFI_YELLOW | EFI_BACKGROUND_BLUE);
	Print(L"READY.\n");
	for (;;) {
		if (!uefi_call_wrapper(BS->CheckEvent, 1, ST->ConIn->WaitForKey)) {
			r = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
			if (r == EFI_SUCCESS)
				Print(L"Key: %c\n", key.UnicodeChar);
		}
	}

	return EFI_SUCCESS;
}
