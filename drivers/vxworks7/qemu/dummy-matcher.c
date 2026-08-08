/*

Dummy driver to print all the devices passed to us for matching.

If a device tree entry has a disable status like so:

mydevice {
    status = "disabled";
};

That entry will not show up in the scan.

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/buslib/vxbVirtLib.h>
#include <vxbMiiLib.h>

LOCAL int
matchprobefdt(VXB_DEV_ID dev)
{
	VXB_FDT_DEV *fdtdev;
	const char *name;

	name = "<nil>";
	fdtdev = vxbFdtDevGet(dev);
	if (fdtdev)
		name = fdtdev->name;
	printf("fdt probe: %p %s\n", dev, name);
	return 0;
}

LOCAL int
matchprobepci(VXB_DEV_ID dev)
{
	PCI_HARDWARE *pcidev;
	int vid, did;

	vid = did = 0;
	pcidev = vxbDevIvarsGet(dev);
	if (pcidev) {
		vid = pcidev->pciVendId;
		did = pcidev->pciDevId;
	}
	printf("pci probe: %p vid: %#04x did:%#04x\n", dev, vid, did);
	return 0;
}

LOCAL int
matchprobevirt(VXB_DEV_ID dev)
{
	const char *name;

	name = vxbDevNameGet(dev);
	if (!name)
		name = "<nil>";
	printf("virt probe: %p %s\n", dev, name);
	return 0;
}

LOCAL int
matchproberoot(VXB_DEV_ID dev)
{
	printf("root probe: %p\n", dev);
	return 0;
}

LOCAL int
matchprobenexus(VXB_DEV_ID dev)
{
	printf("nexus probe: %p\n", dev);
	return 0;
}

LOCAL int
matchprobemii(VXB_DEV_ID dev)
{
	printf("mii probe: %p\n", dev);
	return 0;
}

LOCAL VXB_DRV_METHOD matchdevfdt[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), matchprobefdt },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD matchdevpci[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), matchprobepci },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD matchdevvirt[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), matchprobevirt },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD matchdevroot[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), matchproberoot },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD matchdevnexus[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), matchprobenexus },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD matchdevmii[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), matchprobemii },
	VXB_DEVMETHOD_END,
};

VXB_DRV matchdrvfdt = {
	{ NULL },
	"dummy-matcher-fdt",
	"Dummy matcher FDT driver",
	VXB_BUSID_FDT,
	0,
	0,
	matchdevfdt,
	NULL,
};

VXB_DRV matchdrvpci = {
	{ NULL },
	"dummy-matcher-pci",
	"Dummy matcher PCI driver",
	VXB_BUSID_PCI,
	0,
	0,
	matchdevpci,
	NULL,
};

VXB_DRV matchdrvvirt = {
	{ NULL },
	"dummy-matcher-virt",
	"Dummy matcher Virt driver",
	VXB_BUSID_VIRT,
	0,
	0,
	matchdevvirt,
	NULL,
};

VXB_DRV matchdrvroot = {
	{ NULL },
	"dummy-matcher-root",
	"Dummy matcher Root driver",
	VXB_BUSID_ROOT,
	0,
	0,
	matchdevroot,
	NULL,
};

VXB_DRV matchdrvnexus = {
	{ NULL },
	"dummy-matcher-nexus",
	"Dummy matcher Nexus driver",
	VXB_BUSID_NEXUS,
	0,
	0,
	matchdevnexus,
	NULL,
};

VXB_DRV matchdrvmii = {
	{ NULL },
	"dummy-matcher-mii",
	"Dummy matcher MII driver",
	VXB_BUSID_MII,
	0,
	0,
	matchdevmii,
	NULL,
};

VXB_DRV_DEF(matchdrvfdt)
VXB_DRV_DEF(matchdrvpci)
VXB_DRV_DEF(matchdrvvirt)
VXB_DRV_DEF(matchdrvroot)
VXB_DRV_DEF(matchdrvnexus)
VXB_DRV_DEF(matchdrvmii)

void
matchdrvadd(void)
{
	vxbDrvAdd(&matchdrvfdt);
	vxbDrvAdd(&matchdrvpci);
	vxbDrvAdd(&matchdrvvirt);
	vxbDrvAdd(&matchdrvroot);
	vxbDrvAdd(&matchdrvnexus);
	vxbDrvAdd(&matchdrvmii);
}
