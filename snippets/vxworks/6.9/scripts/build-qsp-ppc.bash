#!/bin/bash

set -e
set -x

BOOTAPP=""
DEBUG=""

ROMFS=""

SMP=""
BSP="wrQspPpc"
EXTRA_BUILD_FLAGS=""
PROFILE="PROFILE_STANDALONE_DEVELOPMENT"

COMPILER="diab"

VSB_COMPONENTS=""
VSB_CONFIGURATION="_WRS_CONFIG_DEBUG_FLAG=y _WRS_CONFIG_COMPONENT_IPSSH=y"
VSB_BUILD_OPTIONS=""

VIP_BUILD_OPTIONS=""
VIP_COMPONENTS="INCLUDE_RAM_DISK INCLUDE_IPCOM_USE_RAM_DISK INCLUDE_ROMFS \
	INCLUDE_USR_NVRAM_IMPLEMENTATION INCLUDE_SYS_WARM_FD \
	INCLUDE_PING INCLUDE_IPPING_CMD INCLUDE_PING6 INCLUDE_IPPING6_CMD \
	INCLUDE_IFCONFIG INCLUDE_IPIFCONFIG_CMD \
	INCLUDE_NETSTAT INCLUDE_ROUTECMD \
	INCLUDE_IPFTP_CMD INCLUDE_IPFTPC INCLUDE_IPTFTP_CLIENT_CMD INCLUDE_IPTFTPC INCLUDE_IPTFTP_COMMON INCLUDE_IPTFTPS INCLUDE_IPFTPS \
	INCLUDE_SSH \
	INCLUDE_IPVERSION_CMD INCLUDE_IPD_CMD INCLUDE_IPCOM_CPU_CMD INCLUDE_IPCOM_SYSLOGD_CMD INCLUDE_IPCOM_SYSVAR_CMD INCLUDE_IPSOCKPERF_CMD \
	INCLUDE_ISR_SHOW INCLUDE_VXBUS_SHOW INCLUDE_MEM_EDR_SHOW INCLUDE_TIMER_SYS_SHOW INCLUDE_PCI_BUS_SHOW"
VIP_BUNDLES="BUNDLE_STANDALONE_SHELL BUNDLE_EDR BUNDLE_POSIX"

VIP_UNWANTED_COMPONENTS=""

VIP_PARAMETERS="
DEFAULT_BOOT_LINE | \"gei(0,0)host:vxWorks e=10.0.2.15 h=10.0.2.2 g=10.0.2.2 u=target pw=vxTarget o=gei\"
BOOTROM_DIR | \"/ram\"
NV_RAM_SIZE | 0x8000
RAM_DISK_SIZE | 0x2000000
IPCOM_KEY_DB_KEY_ENCRYPTING_KEY | \"test\"
"

VSB="${BSP}_vsb"
VIP="${BSP}_vip"

init_variables() {
	if [ "$BOOTAPP" != "" ]; then
		SMP=""
		PROFILE="PROFILE_BOOTAPP"
		VIP="${BSP}_bootapp_vip"
		VIP_COMPONENTS="INCLUDE_RAM_DISK INCLUDE_IPCOM_USE_RAM_DISK INCLUDE_ROMFS \
			INCLUDE_USR_NVRAM_IMPLEMENTATION INCLUDE_SYS_WARM_FD"
		VIP_BUNDLES=""
		VIP_PARAMETERS="$VIP_PARAMETERS
			BOOTROM_DIR | \"/ram\"
			NV_RAM_SIZE | 0x8000
			RAM_DISK_SIZE | 0x2000000
			IPCOM_KEY_DB_KEY_ENCRYPTING_KEY | \"test\""
	fi

	BUILD_FLAGS="-bsp $BSP $SMP $EXTRA_BUILD_FLAGS"
	if [ "$DEBUG" != "" ]; then
		BUILD_FLAGS="$BUILD_FLAGS -debug"
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

	if [ "$VSB_COMPONENTS" != "" ]; then
		vxprj vsb add component $VSB_COMPONENTS
	fi

	if [ "$VSB_CONFIGURATION" != "" ]; then
		for configuration in $VSB_CONFIGURATION; do
			vxprj vsb config -add "$configuration" -s
		done
	fi

	make $VSB_BUILD_OPTIONS
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
