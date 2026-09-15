#!/bin/sh

KVM="-enable-kvm"
SOUND="-device sb16 -device adlib"
NET="-net nic,model=pcnet -net user"
DISK="-fda floppy.img -hda freedos.img"
CDROM="-cdrom FD14LIVE.iso"
MEM="-m 32"

# boot from cdrom and run the installer
#qemu-system-i386 $KVM $MEM $SOUND $NET $CDROM $DISK -boot order=d

# boot from hard drive after installation
qemu-system-i386 $KVM $MEM $SOUND $NET $CDROM $DISK -boot order=c

