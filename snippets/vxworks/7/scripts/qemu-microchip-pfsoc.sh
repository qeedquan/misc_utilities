#!/bin/sh

set -x

QEMU="qemu-system-riscv64"
MACHINE="-M microchip-icicle-kit"
DUMPDTB=""
DTB="microchip-mpfs-icicle-kit.dtb"
CONSOLE="-nographic"
NET="-nic user -nic tap,ifname=tap0,script=no"
SERIAL="-serial null -serial mon:stdio"
KERNEL="uVxWorks"
BOOTARGS="fs(0,0)host:vxWorks h=10.0.2.1 e=10.0.2.4:ffffff00 g=10.0.2.1 u=target pw=vxTarget f=0x01"
MISC=""

if [ "$DUMPDTB" != "" ]; then
	MACHINE="-machine dumpdtb=$DUMPDTB $MACHINE"
fi

if [ "$DTB" != "" ]; then
	MACHINE="$MACHINE -dtb $DTB"
fi

$QEMU $MACHINE $DTB $NET $CONSOLE $SERIAL $MISC -kernel $KERNEL -append "$BOOTARGS"
