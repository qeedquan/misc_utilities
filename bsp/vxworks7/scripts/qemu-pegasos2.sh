QEMU="qemu-system-ppc"
MACH="-machine pegasos2"
KERNEL="pegasos2_vip/default/vxWorks"
EEPROM="eeprom.img"

#HD="-hda file.img"
SIO="-serial stdio"
GFX="-display none"
WDT="-device i6300esb -watchdog-action poweroff"
NET="-device e1000,netdev=net0,mac=00:00:e8:01:02:03 -netdev user,id=net0,hostfwd=tcp::10023-:23,hostfwd=tcp::12345-:2345 -device ne2k_pci,netdev=net1,mac=00:00:e8:01:02:05 -netdev user,id=net1"
MISC="-device isa-m48t59"

# To connect to the monitor
# socat -,echo=0,icanon=0 unix-connect:qemu-monitor-socket
MONITOR="-monitor unix:qemu-monitor-socket,server,nowait"

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
#TRACE="-d trace:m48t*"
#DEBUG="-s -S"

$QEMU $MACH $HD $SIO $GFX $NET $WDT $NVRAM $MISC $MONITOR -kernel $KERNEL $TRACE $DEBUG
#gdb --args $QEMU $MACH $HD $SIO $GFX $NET $WDT $NVRAM $MISC $MONITOR -kernel $KERNEL $TRACE 

