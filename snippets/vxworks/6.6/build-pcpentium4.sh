#!/bin/sh

set -x
set -e

ROMFS=""
BSP="pcPentium4"

COMPS="INCLUDE_STANDALONE_SYM_TBL \
	INCLUDE_CDROMFS \
	INCLUDE_PING \
	INCLUDE_IPPING_CMD \
	INCLUDE_IPIFCONFIG_CMD \
	INCLUDE_IFCONFIG \
	INCLUDE_RAM_DISK \
	INCLUDE_RAMDRV \
	INCLUDE_DOSFS \
	INCLUDE_IPFTP_CMD \
	INCLUDE_PCI_BUS_SHOW \
	INCLUDE_IPCOM_USE_RAM_DISK"

copy_romfs() {
	if [ "$ROMFS" != "" ]; then
		cp -Rvf $ROMFS romfs
	fi
}

build_bsp() {
	rm -rf $BSP
	vxprj create -force -profile PROFILE_DEVELOPMENT $BSP gnu $BSP
	cd $BSP
	copy_romfs
	vxprj component add $COMPS
	vxprj build
}

build_bsp
