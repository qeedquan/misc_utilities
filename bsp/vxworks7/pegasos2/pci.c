#include "dat.h"
#include "fns.h"

#define csr8r(c, a) vxbRead8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))))
#define csr16r(c, a) vxbRead16(c->reghandle, (UINT16 *)(((char *)(c)->regbase) + ((a))))
#define csr32r(c, a) vxbRead32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))))

#define csr8w(c, a, v) vxbWrite8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))), v)
#define csr16w(c, a, v) vxbWrite16(c->reghandle, (UINT16 *)(((char *)(c)->regbase) + ((a))), v)
#define csr32w(c, a, v) vxbWrite32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))), v)

typedef struct {
	// must be first field
	PCI_ROOT_CHAIN rootchain;

	PCI_ROOT_RES rootres;
	struct fdtPciIntr intr;

	VXB_DEV_ID dev;
	int id;

	VXB_RESOURCE *res;
	void *reghandle;
	VIRT_ADDR regbase;

	SEM_ID sem;
} PGS2_PCI_CTLR;

LOCAL VXB_FDT_DEV_MATCH_ENTRY pgs2pcimatch[] = {
	{ "pci", NULL },
	{},
};

STATUS
pgs2pciaddio(VXB_DEV_ID dev, VXB_RESOURCE *res, VXB_RESOURCE_ADR *resadr)
{
	VXB_FDT_PCI_RANGE iospace;
	VXB_FDT_DEV *fdtdev;
	PCI_HARDWARE *pcidev;
	VXB_DEV_ID parent;

	parent = vxbDevParent(dev);
	fdtdev = vxbFdtDevGet(parent);
	pcidev = vxbDevIvarsGet(dev);

	if (vxbFdtPciRanges(fdtdev, FDT_PCI_SPA_IO, &iospace) != OK)
		return ERROR;

	// manually add resources because the PCI BARs is not populated for this device
	res->pRes = resadr;
	res->id = VXB_RES_ID_CREATE(VXB_RES_IO, 0);
	resadr->start = iospace.baseParent;
	resadr->size = iospace.len & 0xffffffff;
	if (vxbResourceAdd(&pcidev->vxbResList, res) != OK)
		return ERROR;

	return OK;
}

LOCAL void
pgs2pcifree(PGS2_PCI_CTLR *ctlr)
{
	if (!ctlr)
		return;

	vxbResourceFree(ctlr->dev, ctlr->res);
	semDelete(ctlr->sem);
	vxbMemFree(ctlr);
}

LOCAL STATUS
pgs2pciprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, pgs2pcimatch, NULL);
}

LOCAL STATUS
pgs2pciattach(VXB_DEV_ID dev)
{
	PGS2_PCI_CTLR *ctlr;
	VXB_FDT_DEV *fdtdev;
	VXB_RESOURCE_ADR *resadr;
	const UINT32 *prop;
	int proplen;

	VXB_FDT_PCI_RANGE iospace, memspace, prememspace, premem64space;
	UINT32 basebus, endbus;
	PCI_ROOT_CHAIN *rootchain;
	PCI_ROOT_RES *rootres;
	struct barRes bres[PCI_ROOT_RES_MAX_IDX];

	if (dev == NULL)
		return ERROR;

	fdtdev = vxbFdtDevGet(dev);
	if (fdtdev == NULL)
		return ERROR;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	prop = vxFdtPropGet(fdtdev->offset, "pci-bridge-number", &proplen);
	if (prop == NULL || proplen != 4)
		goto error;
	ctlr->id = vxFdt32ToCpu(*prop) & 1;

	ctlr->dev = dev;
	ctlr->sem = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
	ctlr->res = vxbResourceAlloc(dev, VXB_RES_MEMORY, 1);
	if (ctlr->sem == SEM_ID_NULL || ctlr->res == NULL)
		goto error;

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual;
	ctlr->reghandle = resadr->pHandle;

	memset(&memspace, 0, sizeof(memspace));
	memset(&prememspace, 0, sizeof(prememspace));
	memset(&premem64space, 0, sizeof(premem64space));
	memset(&iospace, 0, sizeof(iospace));
	if (vxbFdtPciRanges(fdtdev, FDT_PCI_SPA_MEM, &memspace) != OK)
		memspace.len = 0;

	if (vxbFdtPciRanges(fdtdev, FDT_PCI_SPA_MEM_PRE, &prememspace) != OK)
		prememspace.len = 0;

	if (vxbFdtPciRanges(fdtdev, FDT_PCI_SPA_MEM64_PRE, &premem64space) != OK)
		premem64space.len = 0;

	if (vxbFdtPciRanges(fdtdev, FDT_PCI_SPA_IO, &iospace) != OK)
		iospace.len = 0;

	if (vxbFdtPciIntrInfo(fdtdev, &ctlr->intr) != OK)
		goto error;

	if (vxbFdtPciGetBusRange(fdtdev, &basebus, &endbus) != OK)
		goto error;

	rootchain = &ctlr->rootchain;
	rootres = &ctlr->rootres;
	rootchain->segCount = 1;
	rootchain->autoConfig = TRUE;
	rootchain->pRootRes = rootres;

	rootres->baseBusNumber = basebus & 0xff;
	rootres->endBusNumber = endbus & 0xff;

	memset(bres, 0, sizeof(bres));
	bres[PCI_ROOT_RES_IO_IDX].base = iospace.basePci;
	bres[PCI_ROOT_RES_IO_IDX].baseParent = iospace.baseParent;
	bres[PCI_ROOT_RES_IO_IDX].adjust = iospace.baseParent - iospace.basePci;
	bres[PCI_ROOT_RES_IO_IDX].len = iospace.len;

	bres[PCI_ROOT_RES_MEM32_IDX].base = memspace.basePci;
	bres[PCI_ROOT_RES_MEM32_IDX].baseParent = memspace.baseParent;
	bres[PCI_ROOT_RES_MEM32_IDX].len = memspace.len;
	bres[PCI_ROOT_RES_MEM32_IDX].adjust = memspace.baseParent - memspace.basePci;

	bres[PCI_ROOT_RES_PREMEM32_IDX].base = prememspace.basePci;
	bres[PCI_ROOT_RES_PREMEM32_IDX].baseParent = prememspace.baseParent;
	bres[PCI_ROOT_RES_PREMEM32_IDX].len = prememspace.len;
	bres[PCI_ROOT_RES_PREMEM32_IDX].adjust = prememspace.baseParent - prememspace.basePci;

	bres[PCI_ROOT_RES_PREMEM64_IDX].base = premem64space.basePci;
	bres[PCI_ROOT_RES_PREMEM64_IDX].baseParent = premem64space.baseParent;
	bres[PCI_ROOT_RES_PREMEM64_IDX].len = premem64space.len;
	bres[PCI_ROOT_RES_PREMEM64_IDX].adjust = premem64space.baseParent - premem64space.basePci;

	vxbPciSetBarResPool(rootres, bres);
	vxbDevSoftcSet(dev, ctlr);

	if (vxbPciAutoConfig(dev) != OK)
		goto error;

	return OK;

error:
	pgs2pcifree(ctlr);
	return ERROR;
}

LOCAL STATUS
pgs2pcicfgread(VXB_DEV_ID dev, PCI_HARDWARE *pcidev, UINT32 off, UINT32 width, void *data)
{
	static const int cfgaddr[] = { MV64360_PCI1_CFG_ADRS, MV64360_PCI0_CFG_ADRS };
	static const int dataddr[] = { MV64360_PCI1_CFG_DAT_VIRT_REG, MV64360_PCI0_CFG_DAT_VIRT_REG };

	PGS2_PCI_CTLR *ctlr;
	UINT32 addr, val;

	ctlr = vxbDevSoftcGet(dev);
	addr = vxbPciConfigBdfPack(pcidev->pciBus, pcidev->pciDev, pcidev->pciFunc) | (off & 0xfc) | (1U << 31);
	addr = htole32(addr);

	semTake(ctlr->sem, WAIT_FOREVER);
	csr32w(ctlr, cfgaddr[ctlr->id], addr);
	val = csr32r(ctlr, dataddr[ctlr->id]);
	val = le32toh(val);
	semGive(ctlr->sem);

	switch (width) {
	case 1:
		*(UINT8 *)data = (val >> ((off & 3) * 8)) & 0xff;
		break;

	case 2:
		*(UINT16 *)data = (val >> ((off & 2) * 8)) & 0xffff;
		break;

	case 4:
		*(UINT32 *)data = val;
		break;

	default:
		return ERROR;
	}

	return OK;
}

LOCAL STATUS
pgs2pcicfgwrite(VXB_DEV_ID dev, PCI_HARDWARE *pcidev, UINT32 off, UINT32 width, UINT32 data)
{
	static const int cfgaddr[] = { MV64360_PCI1_CFG_ADRS, MV64360_PCI0_CFG_ADRS };
	static const int dataddr[] = { MV64360_PCI1_CFG_DAT_VIRT_REG, MV64360_PCI0_CFG_DAT_VIRT_REG };

	PGS2_PCI_CTLR *ctlr;
	UINT32 addr;
	STATUS status;

	status = OK;
	ctlr = vxbDevSoftcGet(dev);
	addr = vxbPciConfigBdfPack(pcidev->pciBus, pcidev->pciDev, pcidev->pciFunc) | (off & 0xfc) | (1U << 31);
	addr = htole32(addr);

	semTake(ctlr->sem, WAIT_FOREVER);
	csr32w(ctlr, cfgaddr[ctlr->id], addr);
	switch (width) {
	case 1:
		csr8w(ctlr, dataddr[ctlr->id] + (off & 0x3), data & 0xff);
		break;

	case 2:
		data = htole16(data);
		csr16w(ctlr, dataddr[ctlr->id] + (off & 0x2), data & 0xffff);
		break;

	case 4:
		data = htole32(data);
		csr32w(ctlr, dataddr[ctlr->id], data);
		break;

	default:
		status = ERROR;
		break;
	}
	semGive(ctlr->sem);

	return status;
}

LOCAL STATUS
pgs2pciintassign(VXB_DEV_ID dev, PCI_HARDWARE *pcidev, int pin, UINT8 *interrupt, VXB_INTR_ENTRY *intrentry)
{
	PGS2_PCI_CTLR *ctlr;

	if (dev == NULL || pcidev == NULL || intrentry == NULL)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	if (ctlr == NULL)
		return ERROR;

	return vxbFdtPciIntrGet(&ctlr->intr, pcidev->pciBus, pcidev->pciDev, pcidev->pciFunc, pin, interrupt, intrentry);
}

LOCAL VXB_RESOURCE *
pgs2pciresalloc(VXB_DEV_ID dev, VXB_DEV_ID child, UINT32 id)
{
	VXB_RESOURCE *res;
	VXB_RESOURCE_ADR *resadr;
	PCI_HARDWARE *pcidev;

	if (child == NULL)
		return NULL;

	pcidev = vxbDevIvarsGet(child);
	res = vxbResourceFind(&pcidev->vxbResList, id);
	if (res == NULL)
		return NULL;

	switch (VXB_RES_TYPE(res->id)) {
	case VXB_RES_MEMORY:
	case VXB_RES_IO:
		resadr = res->pRes;

		// already mapped
		if (resadr->virtual)
			break;

		if (vxbRegMap(res) != OK)
			return NULL;

		resadr->pHandle = VXB_HANDLE_SWAP(resadr->pHandle);
		break;

	case VXB_RES_IRQ:
		if (vxbIntMap(res) != OK)
			return NULL;
		break;

	default:
		return NULL;
	}

	return res;
}

LOCAL STATUS
pgs2pciresfree(VXB_DEV_ID dev, VXB_DEV_ID child, VXB_RESOURCE *res)
{
	int restype;

	if (child == NULL)
		return ERROR;

	restype = VXB_RES_TYPE(res->id);
	if (restype == VXB_RES_MEMORY || restype == VXB_RES_IO)
		return vxbRegUnmap(res);

	return OK;
}

LOCAL VXB_RESOURCE_LIST *
pgs2pcireslistget(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	PCI_HARDWARE *pcidev;

	if (dev == NULL)
		return NULL;

	pcidev = vxbDevIvarsGet(child);
	if (pcidev == NULL)
		return NULL;

	return &pcidev->vxbResList;
}

LOCAL VXB_DRV_METHOD pgs2pcimethods[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), pgs2pciprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), pgs2pciattach },
	{ VXB_DEVMETHOD_CALL(vxbDevIoctl), vxbPciBusIoctl },
	{ VXB_DEVMETHOD_CALL(vxbPciCfgRead), pgs2pcicfgread },
	{ VXB_DEVMETHOD_CALL(vxbPciCfgWrite), pgs2pcicfgwrite },
	{ VXB_DEVMETHOD_CALL(vxbPciIntAssign), pgs2pciintassign },
	{ VXB_DEVMETHOD_CALL(vxbResourceFree), pgs2pciresfree },
	{ VXB_DEVMETHOD_CALL(vxbResourceAlloc), (FUNCPTR)pgs2pciresalloc },
	{ VXB_DEVMETHOD_CALL(vxbResourceListGet), (FUNCPTR)pgs2pcireslistget },
	VXB_DEVMETHOD_END,
};

VXB_DRV pgs2pcidrv = {
	{ NULL },
	"pgs2-pci",
	"Pegasos 2 PCI Controller Driver",
	VXB_BUSID_FDT,
	0,
	0,
	pgs2pcimethods,
	NULL,
};

VXB_DRV_DEF(pgs2pcidrv)

void
pgs2pcilink(void)
{
}
