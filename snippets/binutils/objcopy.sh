# Convert a raw binary file to ELF
objcopy --input-target=binary --output-target=elf32-little in.rom out.elf
objcopy --input-target=binary --output-target=elf32-big in.rom out.elf
objcopy --input-target=binary --output-target=elf32-powerpc in.rom out.elf
