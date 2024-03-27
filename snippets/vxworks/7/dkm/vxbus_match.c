#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbVirtLib.h>
#include <hwif/buslib/pciDefines.h>
#include <vxbMiiLib.h>

const char *
bustypestr(VXB_BUSTYPE_ID bustype)
{
	static const VXB_BUSTYPE_ID types[] = {VXB_BUSID_ROOT, VXB_BUSID_NONE, VXB_BUSID_NEXUS, VXB_BUSID_PCI, VXB_BUSID_FDT, VXB_BUSID_VIRT, VXB_BUSID_MII};
	static const char *strings[] = {"root", "none", "nexus", "pci", "fdt", "virt", "mii"};

	size_t i;

	for (i = 0; i < NELEMENTS(types); i++) {
		if (VXB_BUSID_MATCH(bustype, types[i]))
			return strings[i];
	}
	return "unknown";
}

BOOL
matcher(VXB_DEV_ID dev, void *param)
{
	PCI_HARDWARE *pcidev;
	VXB_DEV_ID parent;
	VXB_BUSTYPE_ID bustype;
	const char *name, *nameaddr;
	UINT16 cfgvid, cfgdid, cfgstat;
	UINT8 cfgrev;

	parent = vxbDevParent(dev);
	bustype = vxbDevClassGet(dev);
	name = vxbDevNameGet(dev);
	nameaddr = vxbDevNameAddrGet(dev);

	printf("dev=%p parent=%p bustype=%#x (%s) name=%s nameaddr=%s\n",
	       dev, parent, bustype, bustypestr(bustype), name, nameaddr);

	if (VXB_BUSID_MATCH(bustype, VXB_BUSID_PCI)) {
		pcidev = vxbDevIvarsGet(dev);

		cfgvid = cfgdid = 0;
		cfgstat = 0;
		cfgrev = 0;
		VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_VENDOR_ID, 2, cfgvid);
		VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_DEVICE_ID, 2, cfgdid);
		VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_STATUS, 2, cfgstat);
		VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_REVISION, 1, cfgrev);

		printf("  bus %d dev %d func %d int %d vid %#04x did %#04x\n",
		       pcidev->pciBus, pcidev->pciDev, pcidev->pciFunc, pcidev->pciIntPin, pcidev->pciVendId, pcidev->pciDevId);
		printf("  cfgvid %#04x cfgdid %#04x cfgstat %#04x, cfgrev %d\n",
		       cfgvid, cfgdid, cfgstat, cfgrev);
	}

	return FALSE;

	(void)param;
}

void
vxbusmatch(void)
{
	vxbAcquireByMatch(matcher, NULL);
}
