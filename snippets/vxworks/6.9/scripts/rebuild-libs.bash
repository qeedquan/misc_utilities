#!/bin/bash

for i in boot util; do
	make -C $WIND_BASE/target/src/$i CPU=PPC32 TOOL=diab rclean
	make -C $WIND_BASE/target/src/$i CPU=PPC32 TOOL=diab
done

make -C $WIND_BASE/target/src/ipnet CPU=PPC32 TOOL=diab rclean
make -C $WIND_BASE/target/src/ipnet CPU=PPC32 TOOL=diab COMPONENT_IPCOM=true COMPONENT_IPMCRYPTO=false COMPONENT_IPCRYPTO=true COMPONENT_IPXINC=true COMPONENT_IPSSL=true COMPONENT_IPSSH=true IPFTP_TLS=true ADDED_CFLAGS='-DIPCOM_USE_FILE=3 -DIPCRYPTO'

