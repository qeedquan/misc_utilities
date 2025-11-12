#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>
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
		{ "COMMAND", PCI_CFG_COMMAND, 2 },
		{ "STATUS", PCI_CFG_STATUS, 2 },
		{ "CLASS", PCI_CFG_CLASS, 1 },
		{ "HEADER_TYPE", PCI_CFG_HEADER_TYPE, 1 },
		{ "BIST", PCI_CFG_BIST, 1 },
		{ "BASE_ADDRESS_0", PCI_CFG_BASE_ADDRESS_0, 4 },
		{ "BASE_ADDRESS_1", PCI_CFG_BASE_ADDRESS_1, 4 },
		{ "BASE_ADDRESS_2", PCI_CFG_BASE_ADDRESS_2, 4 },
		{ "BASE_ADDRESS_3", PCI_CFG_BASE_ADDRESS_3, 4 },
		{ "BASE_ADDRESS_4", PCI_CFG_BASE_ADDRESS_4, 4 },
		{ "BASE_ADDRESS_5", PCI_CFG_BASE_ADDRESS_5, 4 },
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

	printf("(dev=%p) %02d.%02d.%d (%04x:%04x) [int: %d]\n",
	    dev, pcidev->pciBus, pcidev->pciDev, pcidev->pciFunc, pcidev->pciVendId, pcidev->pciDevId, pcidev->pciIntPin);

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

LOCAL UINT32
readcfg(VXB_DEV_ID vxbdev, UINT32 bus, UINT32 dev, UINT32 func, UINT32 off)
{
	UINT32 val32;
	PCI_HARDWARE hardinfo;

	memset(&hardinfo, 0, sizeof(hardinfo));
	hardinfo.pciBus = bus;
	hardinfo.pciDev = dev;
	hardinfo.pciFunc = func;
	VXB_PCI_CFG_READ(vxbdev, &hardinfo, off, 4, &val32);
	return val32;
}

size_t
pciscancfg(VXB_DEV_ID vxbdev, UINT32 *bdf)
{
	UINT32 bus, dev, func;
	UINT32 data, vid, did;
	size_t nbdf;

	nbdf = 0;
	for (bus = 0; bus < PCI_MAX_BUS; bus++) {
		for (dev = 0; dev < PCI_MAX_DEV; dev++) {
			for (func = 0; func < PCI_MAX_FUNC; func++) {
				data = readcfg(vxbdev, bus, dev, func, PCI_CFG_VENDOR_ID);
				if (data == 0xffffffff)
					continue;

				vid = data & 0xffff;
				did = data >> 16;
				if (bdf)
					bdf[nbdf++] = vxbPciConfigBdfPack(bus, dev, func);
				else
					printf("%02d:%02d.%d: %04x:%04x\n", bus, dev, func, vid, did);
			}
		}
	}
	return nbdf;
}

void
pcidumpcfg(VXB_DEV_ID vxbdev, UINT32 bdf)
{
	UINT8 buf[256];
	UINT32 value;
	int bus, dev, func;
	int off, i;

	bus = VXB_PCI_BUS_GET(bdf);
	dev = VXB_PCI_DEV_GET(bdf);
	func = VXB_PCI_FUNC_GET(bdf);
	printf("%02d:%02d.%d:\n", bus, dev, func);
	for (off = 0; off < 256; off += 4) {
		value = readcfg(vxbdev, bus, dev, func, off);
		le32enc(buf + off, value);
	}

	for (off = 0; off < 256; off += 16) {
		printf("%02x: ", off);
		for (i = 0; i < 16; i++) {
			printf("%02x ", buf[off + i]);
		}
		printf("\n");
	}
	printf("\n\n");
}

void
pcidumpallcfg(VXB_DEV_ID vxbdev)
{
	UINT32 *bdf;
	size_t i, nbdf;

	bdf = calloc(sizeof(*bdf), PCI_MAX_BUS * PCI_MAX_DEV * PCI_MAX_FUNC);
	if (!bdf) {
		printf("Failed to allocate memory\n");
		return;
	}
	nbdf = pciscancfg(vxbdev, bdf);

	printf("%zu devices found:\n", nbdf);
	for (i = 0; i < nbdf; i++)
		pcidumpcfg(vxbdev, bdf[i]);

	free(bdf);
}
