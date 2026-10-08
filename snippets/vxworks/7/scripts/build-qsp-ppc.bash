#!/bin/bash

set -e
set -x

BOOTAPP=""
DEBUG=""
MINIMAL=""

SSH=""

ROMFS=""

CPU="PPCE6500"
SMP="-smp"
BSP="qsp_ppc"
DATA_MODEL="-ilp32"
EXTRA_BUILD_FLAGS=""
PROFILE="PROFILE_DEVELOPMENT"

VSB_LAYERS=""
VSB_COMPONENTS=""
VSB_CONFIGURATION=""
VSB_BUILD_OPTIONS="-j 16"

VIP_BUILD_OPTIONS=""
VIP_COMPONENTS="INCLUDE_ROMFS"
VIP_BUNDLES=""

VSB_UNWANTED_CONFIGURATION=""
VIP_UNWANTED_COMPONENTS=""

VIP_PARAMETERS=""

VIP_FILES=""

VSB="${BSP}_vsb"
VIP="${BSP}_vip"

init_variables() {
	if [ "$BOOTAPP" != "" ]; then
		SMP="-up"
		PROFILE="PROFILE_BOOTAPP"
		VIP="${BSP}_bootapp_vip"
		VIP_COMPONENTS=""
		VIP_BUNDLES=""
	fi

	BUILD_FLAGS="-bsp $BSP -cpu $CPU $SMP $DATA_MODEL $EXTRA_BUILD_FLAGS"
	if [ "$DEBUG" != "" ]; then
		BUILD_FLAGS="$BUILD_FLAGS -debug"
	fi

	if [ "$MINIMAL" != "" ]; then
		BUILD_FLAGS="$BUILD_FLAGS -minimal"
	fi

	if [ "$SSH" != "" ]; then
		VSB_COMPONENTS="$VSB_COMPONENTS IPNET_SSH USER_MANAGEMENT"
		VIP_COMPONENTS="$VIP_COMPONENTS INCLUDE_SSH INCLUDE_IPSSH_CLIENT_CMD"
	fi

	if [ "$PROFILE" != "" ]; then
		PROFILE="-profile $PROFILE"
	fi
}

copy_romfs() {
	if [ "$ROMFS" != "" ]; then
		cp -Rvf $ROMFS romfs
	fi
}

build_vsb() {
	if [ -d "$VSB" ]; then
		return
	fi

	vxprj vsb create -force $BUILD_FLAGS $VSB -S
	pushd $VSB

	if [ "$VSB_LAYERS" != "" ]; then
		vxprj vsb add $VSB_LAYERS
	fi
	
	if [ "$VSB_COMPONENTS" != "" ]; then
		vxprj vsb add component $VSB_COMPONENTS
	fi

	if [ "$VSB_CONFIGURATION" != "" ]; then
		for configuration in $VSB_CONFIGURATION; do
			vxprj vsb config -add "$configuration" -s
		done
	fi

	if [ "$VSB_UNWANTED_CONFIGURATION" != "" ]; then
		for configuration in $VSB_UNWANTED_CONFIGURATION; do
			vxprj vsb config -remove "$configuration" -s
		done
	fi

	vxprj build $VSB_BUILD_OPTIONS
	popd
}

build_vip() {
	vxprj vip create -force $SMP $PROFILE -vsb $VSB $BSP $COMPILER $VIP

	pushd $VIP
	copy_romfs
	
	if [ "$VIP_BUNDLES" != "" ]; then
		vxprj vip bundle add $VIP_BUNDLES
	fi

	if [ "$VIP_COMPONENTS" != "" ]; then
		vxprj vip component add $VIP_COMPONENTS
	fi

	if [ "$VIP_UNWANTED_COMPONENTS" != "" ]; then
		vxprj vip component remove $VIP_UNWANTED_COMPONENTS
	fi

	IFS=$'\n'
	for parameter in $VIP_PARAMETERS; do
		if [[ "$parameter" == "" || "$parameter" =~ ^#.* ]]; then
			continue
		fi
		
		key=$(echo "$parameter" | cut -d '|' -f1 | awk '{$1=$1};1')
		val=$(echo "$parameter" | cut -d '|' -f2 | awk '{$1=$1};1')
		vxprj vip parameter set $key $val
	done

	if [ "$VIP_FILES" != "" ]; then
		vxprj vip file add $VIP_FILES
	fi
	
	vxprj build $VIP_BUILD_OPTIONS
	popd
}

rebuild_vip() {
	pushd $VIP
	vxprj build clean
	vxprj build $VIP_BUILD_OPTIONS
	popd
}

init_variables
build_vsb
build_vip
