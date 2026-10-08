#!/bin/sh

QEMU="qemu-system-i386"

FLOPPY="-fda floppy.img"
SERIAL="-serial stdio"
GFX="-display none"
NET="-device e1000,netdev=net0,mac=00:00:e8:01:02:03 -netdev user,id=net0,hostfwd=tcp::10023-:23,hostfwd=tcp::12345-:2345"

$QEMU $FLOPPY $SERIAL $GFX $NET
