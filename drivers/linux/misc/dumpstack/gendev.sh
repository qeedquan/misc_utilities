#!/bin/sh

module="dumpstack"
major=$(grep $module /proc/devices | cut -d ' ' -f1)
if [ "$major" = "" ];
then
	major=100
fi

echo "Making device major $major"
mknod /dev/$module c $major 0
