#!/bin/sh

QEMU=qemu-system-i386

KERNEL_FILE="itl_x86_vip/default/vxWorks"

MACHINE="-machine q35 -global ICH9-LPC.disable_s3=1"
NUM_CPUS=""
MEM="-m 4G"
KVM="-enable-kvm"

# this will disable the display and redirect all output to serial
CONSOLE="-display none"

SERIAL="-serial mon:stdio"
# enable another serial port piped to a file that can be read/write
# mkfifo serial.in serial.out
# the order is important here as vxworks will use the first one to output as stdio
# SERIAL="-serial pipe:serial $SERIAL"

KERNEL="-kernel $KERNEL_FILE"
NET="-device e1000e,netdev=net0,mac=00:00:e8:01:02:03 -netdev user,id=net0,hostfwd=tcp::10023-:23"

#DEBUG="-s -S"

ARGS="$KVM $KERNEL $BIOS $MACHINE $NUM_CPUS $SERIAL $MEM $DISK $NET $CONSOLE $DEBUG"

$QEMU $ARGS
