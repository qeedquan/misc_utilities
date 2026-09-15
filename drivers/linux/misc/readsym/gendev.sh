#!/bin/sh

module="readsym"
major=$(grep $module /proc/devices | cut -d ' ' -f1)
if [ "$major" = "" ];
then
	major=100
fi

echo "Making device major $major"
mknod /dev/$module c $major 0
