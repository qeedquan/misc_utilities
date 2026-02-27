# https://wiki.osdev.org/UEFI

# provides a virtual FAT formatted filesystem pointing at $VFAT_DIR
# in Linux, this will be seen as /dev/sd* that we can mount
VFAT_DIR="share_directory"
VFAT_ARGS="-drive file=fat:rw:$VFAT_DIR,id=vfat,format=raw"

# provides a SD card backed by $SD_IMG
SD_ARGS="-sd $SD_IMG"
SD_ARGS="-device sd-card,drive=mydrive -drive id=mydrive,if=none,format=raw,file=$SD_IMG"

# By default qemu sets the hostip to 10.0.2.2
# To interact with the host using the standard interface, specify ports to forward
NET="-device e1000e,netdev=net0,mac=00:00:e8:01:02:03 -netdev user,id=net0,hostfwd=tcp::10023-:23,hostfwd=tcp::12345-:2345"
# An alternative is to use TAP support (this needs QEMU to be running as root)
# This allows the guest interface to have an IP assigned (the host can use that IP to connect to it normally)
NET="-netdev tap,id=mynet0,ifname=tap0,script=no,downscript=no -device e1000,netdev=mynet0,mac=52:55:00:d1:55:01"

# use UEFI instead of legacy bios (seabios)
BIOS="-bios /usr/share/qemu/OVMF.fd"

# add custom acpi tables
ACPI="-acpitable sig=SLIC,rev=1,oem_id=LENOVO,oem_table_id=TP-70,oem_rev=00002210,asl_compiler_id=LTP,asl_compiler_rev=00000000,data=SLIC.dat"

# another way to load UEFI (this allows write access to the BIOS so the BIOS need to be somewhere writable)
# for secure boot, the flash has to be able to be written to so separate the code and data section separately, so two files need to be loaded (order matters here)
BIOS="-drive if=pflash,format=raw,unit=0,file=path_to_OVMF_CODE.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=path_to_OVMF_VARS.fd"

# load directly from file image assuming it has the right structure
FILE="-hda file.img"

# serial to stdio
SERIAL_ARGS="-serial stdio"

# in this mode, signals like ctrl+c will be handled by the guest instead of quitting
SERIAL_ARGS="-serial mon:stdio"

# log serial output to file
# use outb to write to 0x3f8 to log output
SERIAL_FILE_ARGS="-device isa-debugcon,iobase=0x3f8,chardev=serialcon -chardev file,path=serial-log.txt,id=serialcon"

# forward host device as a virtual device in QEMU
PCI_SERIAL=-chardev serial,path=/dev/ttyUSB0,id=hostusbserial -device pci-serial,chardev=hostusbserial
# the device can also be a virtual file
PCI_SERIAL=-chardev serial,path=/dev/pts/1,id=hostusbserial -device pci-serial,chardev=hostusbserial

# make a pci serial device that when written to, log to a text file
# the device will show up as /dev/ttyS* in Linux
PCI_SERIAL_FILE=-device pci-serial,chardev=serialcon -chardev file,path=serial-log.txt,id=serialcon

# named pipe
# mkfifo /tmp/io.in /tmp/io.out
SERIAL_ARGS="-serial pipe:io"

# approximate instruction tracing
TRACE="-d in_asm,nochain -singlestep"

# trace watchdog operations
TRACE="-trace \"watchdog_*\""

# don't have an acpi table, this can break some OSes
NO_ACPI="-no-acpi"

# specify a machine to use
# newer than pc
MACH="-machine q35"
# enable SMM
MACH="-machine q35,smm=on"
# old default pc machine
MACH="-machine pc"

# dump the dtb for a machine and quit
# the dtb dumped is different from dumping it in qemu monitor
# this dtb is before any dynamic options are applied so nodes like "memory" arent populated yet
# to get the full dtb in that case, use qemu monitor "dumpdtb" while qemu is running
# there are cases where qemu expects to load a bootloader like uboot and let the bootloader pass the DTB to the OS
# in those cases, dumpdtb option won't work, it only works if qemu passes the DTB directly to the OS
DUMP_DTB="-machine ppce500,dumpdtb=dump.dtb"

# use a custom dtb file, this won't work for all architectures though
USE_DTB="-dtb file.dtb"

# for UEFI, using S3 hibernation can cause the ACPI table to create a hole in the memory map passed through the bootloader
# some OSes cannot handle this and require the whole kernel be placable in the first contiguous RAM descriptor, so disabling this won't create that hole
# for -machine=q35
DISABLE_S3="-global ICH9-LPC.disable_s3=1"
# for -machine pc
DISABLE_S3="-global PIIX4_PM.disable_s3=1"

# To get a full listing of supported traces, use -d trace:help
# Different machines support differenct traces so make sure to use the -M option to specify the machine
# trace memory read/write events to file
TRACE="-trace memory_region_ops*,file=trace.txt"
# trace memory read to output
TRACE="-d trace:memory_region_ops_read"

# add some i2c device at an address (0x24 and 0x30), use i2cdetect command in linux to see it at that address
I2C="-device i2c-echo,address=0x24 -device i2c-ddc,address=0x30"

# debug console logging to a file
# x86 - outb(0xe9, char) to output to the debug console
DEBUGCON="-debugcon file:debugcon.txt"

# log unimplemented events
LOG_EVENTS="-d unimp"

# setup a qemu monitor daemon listening using telnet, user can run qemu monitor commands once connected
# sendkey does not work over serial however, need a pc keyboard connected 
MONITOR="-monitor telnet:127.0.0.1:1234,server,nowait"

# we can also use a unix socket called "qemu-monitor-socket" instead of telnet to connect
# socat -,echo=0,icanon=0 unix-connect:qemu-monitor-socket
MONITOR="-monitor unix:qemu-monitor-socket,server,nowait"

# wait for gdb to connect
GDB="-s -S"

# show the boot menu to choose what medium to boot from
BOOT_MENU="-boot menu=on"

