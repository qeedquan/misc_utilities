#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <endian.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>

typedef struct {
	// must be first field
	DEV_HDR devhdr;

	VXB_DEV_ID dev;

	VXB_RESOURCE *res;
	VXB_RESOURCE *intres;
	int iosdrvnum;

	SEM_ID sem;

	VIRT_ADDR regbase;
	void *reghandle;

	atomic32_t intcnt;

	char stat[256];
	size_t statlen;
	size_t statcnt;
} Ctlr;

#define csr32r(c, a) vxbRead32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))))
#define csr64r(c, a) vxbRead64(c->reghandle, (UINT64 *)(((char *)(c)->regbase) + ((a))))

#define csr32w(c, a, v) vxbWrite32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))), v)
#define csr64w(c, a, v) vxbWrite64(c->reghandle, (UINT64 *)(((char *)(c)->regbase) + ((a))), v)

LOCAL const VXB_PCI_DEV_MATCH_ENTRY edupci[] = {
	{ 0x11e8, 0x1234, NULL },
	{},
};

LOCAL void
edufree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	if (ctlr->sem)
		semDelete(ctlr->sem);

	if (ctlr->res)
		vxbResourceFree(ctlr->dev, ctlr->res);

	if (ctlr->intres)
		vxbResourceFree(ctlr->dev, ctlr->intres);

	if (ctlr->iosdrvnum != ERROR)
		iosDrvRemove(ctlr->iosdrvnum, TRUE);

	vxbMemFree(ctlr);
}

LOCAL void *
eduopen(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	Ctlr *ctlr;

	ctlr = (Ctlr *)devhdr;
	if (semTake(ctlr->sem, NO_WAIT) != OK)
		return (void *)ERROR;

	ctlr->statlen = 0;

	return devhdr;

	(void)name;
	(void)flags;
	(void)mode;
}

LOCAL size_t
readstatus(Ctlr *ctlr, char *buf)
{
	unsigned long intrs;
	char *p;
	size_t l;

	intrs = vxAtomic32Get(&ctlr->intcnt);

	p = buf;
	l = 0;
	l += sprintf(p + l, "#interrupts: %lu\n", intrs);
	l += sprintf(p + l, "00 - ident %xu\n", csr32r(ctlr, 0x00));
	l += sprintf(p + l, "04 w liveness %u\n", csr32r(ctlr, 0x04));
	l += sprintf(p + l, "08 w factorial %u\n", csr32r(ctlr, 0x08));
	l += sprintf(p + l, "20 - status %d\n", csr32r(ctlr, 0x20));
	l += sprintf(p + l, "24 - intr %x\n", csr32r(ctlr, 0x24));
	l += sprintf(p + l, "60 w rintr\n");
	l += sprintf(p + l, "64 - aintr\n");
	l += sprintf(p + l, "80 - dmasrc %lx\n", csr64r(ctlr, 0x80));
	l += sprintf(p + l, "88 - dmadst %lx\n", csr64r(ctlr, 0x88));
	l += sprintf(p + l, "90 - dmacnt %lx\n", csr64r(ctlr, 0x90));
	l += sprintf(p + l, "98 - dmacmd %lx\n", csr64r(ctlr, 0x98));
	return l;
}

LOCAL ssize_t
eduread(void *ctx, char *buf, size_t len)
{
	Ctlr *ctlr;
	size_t nr;

	ctlr = ctx;
	if (ctlr->statlen == 0) {
		ctlr->statlen = readstatus(ctlr, ctlr->stat);
		ctlr->statcnt = 0;
	}

	nr = min(ctlr->statlen - ctlr->statcnt, len);
	if (nr) {
		memcpy(buf, ctlr->stat + ctlr->statcnt, nr);
		ctlr->statcnt += nr;
	} else
		ctlr->statlen = 0;

	return nr;
}

LOCAL ssize_t
eduwrite(void *ctx, const char *buf, size_t len)
{
	Ctlr *ctlr;
	int addr, val;

	if (len != 5)
		return 0;

	ctlr = ctx;
	addr = buf[0];
	val = le32dec(buf + 1);
	switch (addr) {
	case 0x4:
	case 0x8:
	case 0x60:
		csr32w(ctlr, addr, val);
		break;
	}

	return 5;
}

LOCAL int
educlose(void *ctx)
{
	Ctlr *ctlr;

	ctlr = ctx;
	semGive(ctlr->sem);

	return OK;
}

LOCAL void
eduint(void *ctx)
{
	Ctlr *ctlr;

	ctlr = ctx;
	vxAtomic32Add(&ctlr->intcnt, 1);
	csr32w(ctlr, 0x64, 0xFFFFFFFFUL);
}

LOCAL int
eduprobe(VXB_DEV_ID dev)
{
	VXB_PCI_DEV_MATCH_ENTRY *match;
	return vxbPciDevMatch(dev, edupci, &match);
}

LOCAL STATUS
eduattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	char name[MAX_DRV_NAME_LEN];
	VXB_RESOURCE_ADR *resadr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->sem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	ctlr->res = vxbResourceAlloc(dev, VXB_RES_MEMORY, 0);
	ctlr->intres = vxbResourceAlloc(dev, VXB_RES_IRQ, 0);
	ctlr->iosdrvnum = iosDrvInstall(NULL, NULL, eduopen, educlose, eduread, eduwrite, NULL);
	if (ctlr->sem == SEM_ID_NULL || ctlr->res == NULL || ctlr->intres == NULL || ctlr->iosdrvnum == ERROR)
		goto error;

	snprintf(name, sizeof(name), "/edu/%d", vxbDevUnitGet(dev));
	if (iosDevAdd(&ctlr->devhdr, name, ctlr->iosdrvnum) == ERROR)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual;
	ctlr->reghandle = resadr->pHandle;

	vxbIntConnect(ctlr->dev, ctlr->intres, eduint, ctlr);
	vxbIntEnable(ctlr->dev, ctlr->intres);

	return OK;

error:
	edufree(ctlr);
	return ERROR;
}

LOCAL STATUS
edudetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	edufree(ctlr);
	return OK;
}

LOCAL STATUS
edushutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD edudev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), eduprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), eduattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), edudetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), edushutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV edudrv = {
	{ NULL },
	"edu",
	"QEMU EDU driver",
	VXB_BUSID_PCI,
	0,
	0,
	edudev,
	NULL,
};

VXB_DRV_DEF(edudrv)

// For development, build as a DKM file and load it in VxWorks at runtime
STATUS
edudrvadd(void)
{
	return vxbDrvAdd(&edudrv);
}

LOCAL int
dumpfile(const char *name)
{
	int fd;
	char buf[256];
	ssize_t len;

	fd = open(name, O_RDONLY);
	if (fd < 0)
		return -1;

	while ((len = read(fd, buf, sizeof(buf) - 1)) > 0)
		write(1, buf, len);
	printf("\n");

	close(fd);
	return 0;
}

LOCAL int
writevals(const char *name, unsigned addr, unsigned val)
{
	char data[5];
	int fd;
	int len;

	fd = open(name, O_RDWR);
	if (fd < 0)
		return -1;

	data[0] = addr;
	le32enc(data + 1, val);
	len = write(fd, data, sizeof(data));
	close(fd);

	return len;
}

void
edudrvtest(void)
{
	const char *f = "/edu/0";

	dumpfile(f);

	writevals(f, 8, 10);
	dumpfile(f);

	writevals(f, 4, 2);
	dumpfile(f);

	writevals(f, 8, 5);
	dumpfile(f);

	writevals(f, 0x60, 0x55443322);
	dumpfile(f);
}
