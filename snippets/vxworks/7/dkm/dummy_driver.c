// Compile this as a .o file (not .out)
// Linking the .o file into the VIP causes it to get loaded on startup

#include <vxWorks.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>
#include <stdatomic.h>
#include <stdio.h>

atomic_int dummycount;

LOCAL int
dummyprobe(VXB_DEV_ID dev)
{
	return 1;

	(void)dev;
}

LOCAL STATUS
dummyattach(VXB_DEV_ID dev)
{
	dummycount++;
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dummydev[] = {
    {VXB_DEVMETHOD_CALL(vxbDevProbe), dummyprobe},
    {VXB_DEVMETHOD_CALL(vxbDevAttach), dummyattach},
    VXB_DEVMETHOD_END,
};

VXB_DRV dummydrv = {
    {NULL},
    "dummy",
    "Dummy driver",
    VXB_BUSID_PCI,
    0,
    0,
    dummydev,
    NULL,
};

VXB_DRV_DEF(dummydrv)
