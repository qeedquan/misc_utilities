#!/bin/bash

# example arguments:
# hd.img 100M mnt fat
# "mnt" is a temporary directory that is mounted as loopback against the image to copy the data from the "fat" directory to the image

cleanup() {
	sudo umount -q $MNTDIR
	rm -rf $MNTDIR
}

die() {
	echo $@
	cleanup
	rm -f $FILE
	exit 1
}

create_files() {
	mkdir -p $MNTDIR || die "Failed to create mount directory"
	dd if=/dev/zero of=$FILE ibs=$SIZE count=1 || die "Failed to create image"
}

create_filesystem() {
	sed -e 's/\s*\([\+0-9a-zA-Z]*\).*/\1/' << EOF | fdisk $FILE
		o # clear memory partition table
		n # new partition
		p # primary partition
		1 # partition 1
	  	# default - beginning of disk
	  	# end of disk
		t # set partition type
		b # partition type (w95 fat32)
		w # write to disk
EOF

	mkfs.vfat $FILE || die "Failed to create FAT drive"
	sudo umount -q $MNTDIR
	sudo mount $FILE $MNTDIR -o loop || die "Failed to mount FAT drive"
}

copy_files() {
	sudo cp -dR $DIR/. $MNTDIR
}

if [ $# != 4 ]; then
	echo "usage: <hd.img> <size> <mntdir> <dir>"
	exit 2
fi

FILE=$1
SIZE=$2
MNTDIR=$3
DIR=$4

create_files
create_filesystem
copy_files
cleanup
