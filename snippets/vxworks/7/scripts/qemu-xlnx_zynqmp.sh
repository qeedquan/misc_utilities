#!/bin/sh

QEMU=qemu-system-aarch64
MACHINE="-M xlnx-zcu102 -smp 4 -m 4G"
DUMPDTB=""
DTB="xlnx-zcu102-rev-1.1.dtb"
CONSOLE="-nographic"
NET=""
SERIAL="-serial null -serial mon:stdio"
KERNEL="uVxWorks"
BOOTARGS="enet(0,0)host:vxWorks h=192.168.0.2 e=192.168.0.3:ffffff00 g=192.168.0.1 u=target pw=vxTarget"
MISC=""

if [ "$DUMPDTB" != "" ]; then
	MACHINE="-machine dumpdtb=$DUMPDTB $MACHINE"
fi

if [ "$DTB" != "" ]; then
	MACHINE="$MACHINE -dtb $DTB"
fi

$QEMU $MACHINE $DTB $NET $CONSOLE $SERIAL $MISC -kernel $KERNEL -append "$BOOTARGS"
