QEMU="qemu-system-ppc"
MACH="-machine pegasos2"
KERNEL="pegasos2_vip/default/vxWorks"
#HD="-hda file.img"
SIO="-serial stdio"
GFX="-display none"

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

$QEMU $MACH $HD $SIO $GFX -kernel $KERNEL $TRACE $DEBUG
#gdb --args $QEMU $MACH $HD $SIO $GFX -kernel $KERNEL $TRACE 

