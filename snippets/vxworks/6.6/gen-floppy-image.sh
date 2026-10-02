#!/bin/sh

# Generates a floppy image with bootrom.bin that QEMU can use to boot

# create a fat partitioned file
mformat -C -f 1440 -i floppy.img

# copy the bootcode to the first sector
dd if=vxld.bin of=floppy.img conv=notrunc bs=1 skip=62 seek=62 count=448

# copy bootrom.bin as bootrom.sys to the fat partition
mcopy -i floppy.img bootrom.bin ::/bootrom.sys

# copy NVRAM parameters
mcopy -i floppy.img nvram.txt ::/nvram.txt

