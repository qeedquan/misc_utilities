#!/bin/bash

set -e
set -x

BOOTAPP=""
DEBUG=""
MINIMAL=""

SSH="1"

ROMFS=""

CPU="SIMLINUX"
SMP="-smp"
BSP="vxsim_linux"
DATA_MODEL=""
EXTRA_BUILD_FLAGS=""
PROFILE="PROFILE_DEVELOPMENT"

VSB_LAYERS="FBDEV CAMERA"
VSB_COMPONENTS=""
VSB_CONFIGURATION="_WRS_CONFIG_DEBUG_FLAG=y"
VSB_BUILD_OPTIONS="-j 16"

VIP_BUILD_OPTIONS=""
VIP_COMPONENTS="INCLUDE_ROMFS \
	INCLUDE_STANDALONE_SYM_TBL \
	INCLUDE_DEBUG_KPRINTF \
	INCLUDE_PCI_SHOW \
	INCLUDE_ISR_SHOW \
	INCLUDE_ISR_HANDLER_SHOW \
	INCLUDE_ISR_OBJECTS \
	INCLUDE_RTP_CPU_USAGE_CTRL \
	INCLUDE_FDT_SHOW \
	INCLUDE_PINMUX_SYS \
	INCLUDE_TIMER_SYS_SHOW \
	INCLUDE_GPIO_SYS \
	INCLUDE_WATCHDOGDRV \
	INCLUDE_RAM_DISK \
	INCLUDE_RAM_DISK_FORMAT_DOSFS \
	INCLUDE_IFCONFIG \
	INCLUDE_IPIFCONFIG_CMD \
	INCLUDE_INT_SHOW \
	INCLUDE_VXBUS_SHOW \
	INCLUDE_MEM_EDR_SHOW \
	INCLUDE_CORE_DUMP_SHOW \
	INCLUDE_FSINFO_SHOW \
	INCLUDE_VXBUS_IOCTL_SHOW \
	DRV_FSL_I2C \
	DRV_I2C_EEPROM \
	DRV_I2C_RTC \
	INCLUDE_EEPROMDRV \
	DRV_I2C_PCFRTC \
	INCLUDE_PING \
	INCLUDE_IPPING_CMD \
	INCLUDE_IPFTP_CMD \
	INCLUDE_IPTFTP_CLIENT_CMD \
	INCLUDE_FTP6 \
	INCLUDE_TFTP_CLIENT \
	INCLUDE_IPFTPS \
	INCLUDE_IPTFTP_COMMON \
	INCLUDE_IPTFTPS \
	INCLUDE_REGMAP_SYS \
	INCLUDE_MAILBOX_SYS \
	INCLUDE_REGULATOR_SYS \
	INCLUDE_CAMERA_LIB_CORE \
	INCLUDE_LOOPFS \
	INCLUDE_IPDNSC \
	INCLUDE_IPNSLOOKUP_CMD \
	INCLUDE_IPCOM_SYSVAR_CMD \
	DRV_SYS_CON \
	DRV_BUS_VIRT
	"
VIP_BUNDLES="BUNDLE_STANDALONE_SHELL BUNDLE_EDR BUNDLE_POSIX"

VSB_UNWANTED_CONFIGURATION=""
VIP_UNWANTED_COMPONENTS=""

VIP_PARAMETERS="
SEC_VAULT_KEY_ENCRYPTING_PW | \"\"
RAM_DISK_SIZE | 0x8000000
"

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

	BUILD_FLAGS="-bsp $BSP $SMP $DATA_MODEL $EXTRA_BUILD_FLAGS"
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
