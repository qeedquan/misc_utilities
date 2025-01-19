#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/pciDefines.h>

typedef struct {
	const char *name;
	int offset;
	int size;
} PCI_REG;

BOOL
pcimatch(VXB_DEV_ID dev, void *param)
{
	static const PCI_REG pciregs[] = {
	    {"COMMAND", PCI_CFG_COMMAND, 2},
	    {"STATUS", PCI_CFG_STATUS, 2},
	    {"CLASS", PCI_CFG_CLASS, 1},
	    {"HEADER_TYPE", PCI_CFG_HEADER_TYPE, 1},
	    {"BIST", PCI_CFG_BIST, 1},
	    {"BASE_ADDRESS_0", PCI_CFG_BASE_ADDRESS_0, 4},
	    {"BASE_ADDRESS_1", PCI_CFG_BASE_ADDRESS_1, 4},
	    {"BASE_ADDRESS_2", PCI_CFG_BASE_ADDRESS_2, 4},
	    {"BASE_ADDRESS_3", PCI_CFG_BASE_ADDRESS_3, 4},
	    {"BASE_ADDRESS_4", PCI_CFG_BASE_ADDRESS_4, 4},
	    {"BASE_ADDRESS_5", PCI_CFG_BASE_ADDRESS_5, 4},
	};

	const PCI_REG *pcireg;
	PCI_HARDWARE *pcidev;
	VXB_DEV_ID parent;
	VXB_BUSTYPE_ID bustype;
	UINT8 val8;
	UINT16 val16;
	UINT32 val32;
	UINT32 val;
	size_t i, j;

	parent = vxbDevParent(dev);
	bustype = vxbDevClassGet(dev);

	if (!VXB_BUSID_MATCH(bustype, VXB_BUSID_PCI))
		return FALSE;

	pcidev = vxbDevIvarsGet(dev);

	printf("%02d.%02d.%d (%04x:%04x) [int: %d]\n",
	       pcidev->pciBus, pcidev->pciDev, pcidev->pciFunc, pcidev->pciVendId, pcidev->pciDevId, pcidev->pciIntPin);

	for (i = 0; i < NELEMENTS(pciregs); i++) {
		pcireg = pciregs + i;
		switch (pcireg->size) {
		case 1:
			VXB_PCI_BUS_CFG_READ(dev, pcireg->offset, pcireg->size, val8);
			val = val8;
			break;
		case 2:
			VXB_PCI_BUS_CFG_READ(dev, pcireg->offset, pcireg->size, val16);
			val = val16;
			break;
		case 4:
			VXB_PCI_BUS_CFG_READ(dev, pcireg->offset, pcireg->size, val32);
			val = val32;
			break;
		}
		printf("%-16s: %#x\n", pcireg->name, val);
	}

	for (i = 0; i < 256; i += 16) {
		printf("%02x: ", i);
		for (j = 0; j < 16; j++) {
			VXB_PCI_BUS_CFG_READ(dev, i + j, 1, val8);
			printf("%02x ", val8);
		}
		printf("\n");
	}

	printf("\n");

	return FALSE;

	(void)param;
}

void
lspci(void)
{
	vxbAcquireByMatch(pcimatch, NULL);
}
