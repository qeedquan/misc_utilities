#!/bin/sh

# Converts footprint generated file from Kicad PCB viewer to a valid CPL file for JLCPCB

set -e

if [ $# -lt 1 ]; then
	echo "usage: [options] footprint.csv ..."
	exit 0
fi

for FILE in "$@"
do
	sed 's/Ref/Designator/g' -i "$FILE"
	sed 's/PosX/Mid X/g' -i "$FILE"
	sed 's/PosY/Mid Y/g' -i "$FILE"
	sed 's/Rot/Rotation/g' -i "$FILE"
	sed 's/Side/Layer/g' -i "$FILE"
done
