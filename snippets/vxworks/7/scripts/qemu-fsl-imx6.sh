#!/bin/sh

QEMU=qemu-system-arm
MACHINE="-M sabrelite -smp 4 -m 1G"
DUMPDTB=""
DTB="imx6q-sabrelite.dtb"
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
