#!/bin/bash

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
	INCLUDE_IPCOM_USE_RAM_DISK \
	INCLUDE_NFS_CLIENT_ALL"

PARAMETERS="
NFS_USER_ID | 0
NFS_GROUP_ID | 0
"

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
	
	IFS=$'\n'
	for parameter in $PARAMETERS; do
		if [[ "$parameter" == "" || "$parameter" =~ ^#.* ]]; then
			continue
		fi
		
		key=$(echo "$parameter" | cut -d '|' -f1 | awk '{$1=$1};1')
		val=$(echo "$parameter" | cut -d '|' -f2 | awk '{$1=$1};1')
		vxprj parameter set $key $val
	done

	vxprj build
}

build_bsp
