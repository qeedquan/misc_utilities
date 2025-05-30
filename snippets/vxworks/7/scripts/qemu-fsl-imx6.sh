#!/bin/sh

QEMU=qemu-system-arm
MACHINE="-M sabrelite -smp 4 -m 1G"
DUMPDTB=""
DTB="fsl_imx6_vip/default/imx6q-sabrelite.dtb"
CONSOLE="-nographic"
NET=""
SERIAL="-serial null -serial mon:stdio"
KERNEL="fsl_imx6_vip/default/uVxWorks"
BOOTARGS="enet(0,0)host:vxWorks h=10.0.2.2 e=10.0.2.15 g=10.0.2.15 u=target pw=vxTarget"
MISC=""

if [ "$DUMPDTB" != "" ]; then
	MACHINE="-machine dumpdtb=$DUMPDTB $MACHINE"
fi

if [ "$DTB" != "" ]; then
	MACHINE="$MACHINE -dtb $DTB"
fi

$QEMU $MACHINE $DTB $NET $CONSOLE $SERIAL $MISC -kernel $KERNEL -append "$BOOTARGS"
