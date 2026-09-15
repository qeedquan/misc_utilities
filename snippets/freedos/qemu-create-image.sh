#!/bin/sh

qemu-img create freedos.img 512M -f raw
qemu-img create floppy.img 1474560 -f raw
