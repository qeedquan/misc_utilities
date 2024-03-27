#!/bin/sh

# https://labs.windriver.com/downloads/wrsdk-vxworks7-docs/README-qemu-x86-64.html

QEMU=qemu-system-x86_64
TPMDIR=./tpm0

BIOS_FILE="OVMF_CODE.fd"
BIOS_STORE="OVMF_VARS.fd"

MACHINE="-machine q35 -global ICH9-LPC.disable_s3=1"
NUM_CPUS="-smp 4"
MEM="-m 4G"
KVM="-enable-kvm"

# this will disable the display and redirect all output to serial
CONSOLE="-display none"

# debug console logging to a file
# use outb(0xe9, char) to write a character to the debug console
#DEBUGCON="-debugcon file:debugcon.txt"

SERIAL="-serial mon:stdio"
# enable another serial port piped to a file that can be read/write
# mkfifo serial.in serial.out
# the order is important here as vxworks will use the first one to output as stdio
# SERIAL="-serial pipe:serial $SERIAL"

BIOS="-bios $BIOS_FILE"
# for secure boot, need to split the OVMF into a readable and writable portion, code is readonly while the data is writable (to store keys and such)
#BIOS="-drive if=pflash,unit=0,format=raw,readonly=on,file=$BIOS_FILE -drive if=pflash,unit=1,format=raw,file=$BIOS_STORE"

# boot partition, some UEFI firmware will try to save stuff on shutdown and breaks the image upon another boot
# a way around this is to create a image that can be loaded with -hda <hd.img>
#DISK="-device ich9-ahci,id=ahci -drive id=SATA_DISK,file=fat:raw:rw:boot,if=none,format=raw -device ide-hd,drive=SATA_DISK,bus=ahci.0"
BOOT_IMAGE="boot.img"
DISK="-hda $BOOT_IMAGE"

DISK="$DISK -device ich9-usb-uhci1,id=usb-uhci -drive id=USB_DISK,file=fat:raw:rw:disk,if=none,format=raw -device usb-storage,drive=USB_DISK,bus=usb-uhci.0"
# adds a AHCI device
#DISK="$DISK -drive id=disk,file=ata.img,if=none -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0"
# adds a NVME device
#DISK="$DISK -drive file=nvm.img,if=none,id=nvm -device nvme,serial=deadbeef,drive=nvm"

# use TPM
# swtpm socket --tpm2 --tpmstate dir=$TPMDIR --ctrl type=unixio,path=$TPMDIR/swtpm-sock --log level=20 --flags not-need-init,startup-clear
#TPM="-chardev socket,id=chrtpm,path=$TPMDIR/swtpm-sock -tpmdev emulator,id=tpm0,chardev=chrtpm -device tpm-tis,tpmdev=tpm0"

# allows connection to vxworks
# telnet localhost 10023 to get a shell
NET="-device e1000e,netdev=net0,mac=00:00:e8:01:02:03 -netdev user,id=net0,hostfwd=tcp::10023-:23"
#NET="-net nic -net user,hostfwd=tcp::1534-:1534"

#TRACE="-trace memory_region_ops_write,file=trace.txt"

ARGS="$KVM $BIOS $MACHINE $NUM_CPUS $SERIAL $MEM $DISK $NET $CONSOLE $DEBUGCON $TPM"

if [ ! -f "$BOOT_IMAGE" ]; then
	echo "boot image $BOOT_IMAGE does not exist"
	exit 1
fi

$QEMU $ARGS
