/*

Test PCI functions

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <sysLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <endian.h>
#include <hwif/buslib/pciDefines.h>
#include <hwif/buslib/vxbPciLib.h>

LOCAL UINT32
readcfg(UINT32 bus, UINT32 dev, UINT32 func, UINT32 off)
{
	UINT32 addr;

	addr = vxbPciConfigBdfPack(bus, dev, func) | off | (1U << 31);
	sysOutLong(PCI_CONFIG_ADDR, addr);
	return sysInLong(PCI_CONFIG_DATA);
}

size_t
pciscancfg(UINT32 *bdf)
{
	UINT32 bus, dev, func;
	UINT32 data, vid, did;
	size_t nbdf;

	nbdf = 0;
	for (bus = 0; bus < PCI_MAX_BUS; bus++) {
		for (dev = 0; dev < PCI_MAX_DEV; dev++) {
			for (func = 0; func < PCI_MAX_FUNC; func++) {
				data = readcfg(bus, dev, func, PCI_CFG_VENDOR_ID);
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
pcidumpcfg(UINT32 bdf)
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
		value = readcfg(bus, dev, func, off);
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
pcidumpallcfg(void)
{
	UINT32 *bdf;
	size_t i, nbdf;

	bdf = calloc(sizeof(*bdf), PCI_MAX_BUS * PCI_MAX_DEV * PCI_MAX_FUNC);
	if (!bdf) {
		printf("Failed to allocate memory\n");
		return;
	}
	nbdf = pciscancfg(bdf);

	printf("%zu devices found:\n", nbdf);
	for (i = 0; i < nbdf; i++)
		pcidumpcfg(bdf[i]);

	free(bdf);
}
