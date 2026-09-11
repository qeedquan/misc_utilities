#!/bin/sh

# Need the following DTB fixups for the following devices:
# i2c { ... } needs #address-cells, #size-cells, and clock-frequency values so the i2c driver and it's children can be attached properly.

QEMU=qemu-system-ppc64
MACHINE="-M ppce500 -cpu e500mc -smp 4 -m 2G"
DUMPDTB=""
DTB=""
CONSOLE="-nographic"
NET="-device eTSEC,netdev=net0,mac=00:00:e8:01:02:03 -netdev user,id=net0,hostfwd=tcp::10023-:23"
SERIAL="-serial mon:stdio"
KERNEL="uVxWorks"
BOOTARGS="fs(0,0)host:vxWorks h=10.0.2.2 e=10.0.2.15 g=10.0.2.15 u=target pw=vxTarget o=motetsec0 f=0x1"
MISC=""

if [ "$DUMPDTB" != "" ]; then
	MACHINE="-machine dumpdtb=$DUMPDTB $MACHINE"
fi

if [ "$DTB" != "" ]; then
	MACHINE="$MACHINE -dtb $DTB"
fi

$QEMU $MACHINE $DTB $NET $CONSOLE $SERIAL $MISC -kernel $KERNEL -append "$BOOTARGS"
