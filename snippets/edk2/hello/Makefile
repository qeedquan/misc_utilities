EFI_DIR=/usr/lib

INC=-I/usr/include/efi
INC+=-I/usr/include/efi/x86_64

CFLAGS=-fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args

all:
	gcc $(INC) $(CFLAGS) -c main.c main.c
	ld -shared -Bsymbolic -L$(EFI_DIR) -T$(EFI_DIR)/elf_x86_64_efi.lds $(EFI_DIR)/crt0-efi-x86_64.o main.o -o main.so -lgnuefi -lefi
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 main.so main.efi

clean:
	rm -f *.efi *.so *.o
