QEMU="qemu-system-arm"
MACH="-cpu cortex-a15 -machine virt,gic-version=3 -m 2G"
KERNEL="virtarm_vip/default/vxWorks"
EEPROM="eeprom.img"

#HD="-hda file.img"
SIO="-serial stdio"
GFX="-display none"
WDT="-device i6300esb -watchdog-action poweroff"
NET="-device e1000"
if [ -f $EEPROM ]; then
	NVRAM="-drive file=$EEPROM,if=none,format=raw,id=eeprom -device at24c-eeprom,address=0x50,drive=eeprom,writable=yes,rom-size=1048576,address-size=2"
fi

#TRACE="-d trace:vof*,trace:pci*,trace:mv*,trace:pic*"
#TRACE="-d trace:pic_ioport_*" #,trace:pic_set_irq"
#TRACE="-d trace:via_super_io*"
#TRACE="-d trace:ide_*"
#TRACE="-d trace:parallel_*"
#TRACE="-d trace:smbus_*"
#TRACE="-d trace:memory_region_ops_write*"
#TRACE="-d trace:via_pm_io_*"
#TRACE="-d trace:via_pm_*"
#DEBUG="-s -S"

$QEMU $MACH $HD $SIO $GFX $NET $WDT $NVRAM -kernel $KERNEL $TRACE $DEBUG
#gdb --args $QEMU $MACH $HD $SIO $GFX $NET $WDT $NVRAM -kernel $KERNEL $TRACE 

