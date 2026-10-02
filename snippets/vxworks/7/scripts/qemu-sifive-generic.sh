#!/bin/sh

QEMU="qemu-system-riscv64"
MACHINE="-M virt -smp 4 -m 2G"
DUMPDTB=""
DTB=""
CONSOLE="-nographic"
NET="-netdev tap,ifname=tap0,script=no,id=net0 -device e1000,netdev=net0"
SERIAL="-serial mon:stdio"
KERNEL="uVxWorks"
BOOTARGS="gei(0,0)host:vxWorks h=10.0.2.1 e=10.0.2.4:ffffff00 g=10.0.2.1 u=target pw=vxTarget f=0x01"
MISC=""

if [ "$DUMPDTB" != "" ]; then
	MACHINE="-machine dumpdtb=$DUMPDTB $MACHINE"
fi

if [ "$DTB" != "" ]; then
	MACHINE="$MACHINE -dtb $DTB"
fi

$QEMU $MACHINE $DTB $NET $CONSOLE $SERIAL $MISC -kernel $KERNEL -append "$BOOTARGS"
