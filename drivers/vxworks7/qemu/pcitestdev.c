#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ioLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>

typedef struct {
	uint8_t value;
	uint8_t width;
	uint8_t pad0[2];
	uint32_t offset;
	uint32_t data;
	uint32_t count;
	char name[64];
} Hdr;

typedef struct {
	VXB_DEV_ID dev;
	VXB_RESOURCE *res[3];
	VIRT_ADDR regbase[3];
	void *reghandle[3];
	size_t regsize[3];
} Ctlr;

LOCAL UINT8
csr8r(Ctlr *ctlr, int slot, int addr)
{
	return vxbRead8(ctlr->reghandle[slot], (UINT8 *)(ctlr->regbase[slot] + addr));
}

LOCAL UINT32
csr32r(Ctlr *ctlr, int slot, int addr)
{
	return vxbRead32(ctlr->reghandle[slot], (UINT32 *)(ctlr->regbase[slot] + addr));
}

LOCAL const VXB_PCI_DEV_MATCH_ENTRY ptdpci[] = {
	{ 0x0005, 0x1b36, NULL },
	{},
};

LOCAL int
readhdr(Ctlr *ctlr, int slot, Hdr *hdr)
{
	size_t i, len;

	if (ctlr->regsize[slot] < sizeof(*hdr))
		return -EIO;

	hdr->value = csr8r(ctlr, slot, 0);
	hdr->width = csr8r(ctlr, slot, 1);
	hdr->offset = csr32r(ctlr, slot, 4);
	hdr->data = csr32r(ctlr, slot, 8);
	hdr->count = csr32r(ctlr, slot, 12);

	len = sizeof(hdr->name);
	for (i = 0; i < len; i++) {
		hdr->name[i] = csr8r(ctlr, slot, 16 + i);
		if (hdr->name[i] == '\0')
			break;
	}
	hdr->name[len - 1] = '\0';

	return 0;
}

LOCAL void
ptdfree(Ctlr *ctlr)
{
	size_t i;

	if (!ctlr)
		return;

	for (i = 0; i < NELEMENTS(ctlr->res); i++) {
		if (ctlr->res[i])
			vxbResourceFree(ctlr->dev, ctlr->res[i]);
	}

	vxbMemFree(ctlr);
}

LOCAL int
ptdprobe(VXB_DEV_ID dev)
{
	VXB_PCI_DEV_MATCH_ENTRY *match;
	return vxbPciDevMatch(dev, ptdpci, &match);
}

LOCAL STATUS
ptdattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	VXB_RESOURCE_ADR *resadr;
	size_t i;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;

	for (i = 0; i < NELEMENTS(ctlr->res); i++) {
		ctlr->res[i] = vxbResourceAlloc(dev, VXB_RES_MEMORY, i);
		if (!ctlr->res[i])
			ctlr->res[i] = vxbResourceAlloc(dev, VXB_RES_IO, i);

		if (!ctlr->res[i])
			continue;

		resadr = ctlr->res[i]->pRes;
		ctlr->regbase[i] = resadr->virtual;
		ctlr->reghandle[i] = resadr->pHandle;
		ctlr->regsize[i] = resadr->size;
	}

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	ptdfree(ctlr);
	return ERROR;
}

LOCAL STATUS
ptddetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	ptdfree(ctlr);
	return OK;
}

LOCAL STATUS
ptdshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

void
ptdshow(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	Hdr hdr[1];
	size_t i;

	ctlr = vxbDevSoftcGet(dev);
	printf("PCI Test Device %p\n", ctlr->dev);
	for (i = 0; i < NELEMENTS(ctlr->res); i++) {
		printf("BAR%zu: %p (%zu bytes)\n", i, ctlr->regbase[i], ctlr->regsize[i]);
		if (readhdr(ctlr, i, hdr) >= 0) {
			printf("\tValue:  %d\n", hdr->value);
			printf("\tWidth:  %d\n", hdr->width);
			printf("\tOffset: %" PRIu32 "\n", hdr->offset);
			printf("\tData:   %" PRIu32 "\n", hdr->data);
			printf("\tCount:  %" PRIu32 "\n", hdr->count);
			printf("\tName:   %s\n", hdr->name);
			printf("\n");
		}
	}
}

LOCAL VXB_DRV_METHOD ptddev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), ptdprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), ptdattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), ptddetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), ptdshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV ptddrv = {
	{ NULL },
	"ptd",
	"QEMU PCI Test Dev driver",
	VXB_BUSID_PCI,
	0,
	0,
	ptddev,
	NULL,
};

VXB_DRV_DEF(ptddrv)

STATUS
ptddrvadd(void)
{
	return vxbDrvAdd(&ptddrv);
}

void
ptddrvtest(void)
{
	VXB_DEV_ID dev;
	size_t i;

	for (i = 0;; i++) {
		dev = vxbDevAcquireByName("ptd", i);
		if (!dev)
			break;

		ptdshow(dev);

		vxbDevRelease(dev);
	}
}
