#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>

typedef struct {
	DEV_HDR devhdr;
	VXB_DEV_ID dev;

	VXB_RESOURCE *res;
	int iosdrvnum;

	SEM_ID sem;

	VIRT_ADDR regbase;
	void *reghandle;
} Ctlr;

#define csr8r(c, a) vxbRead8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))))
#define csr8w(c, a, v) vxbWrite8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))), v)

LOCAL const VXB_PCI_DEV_MATCH_ENTRY pvpanicpci[] = {
	{ 0x0011, 0x1b36, NULL },
	{},
};

LOCAL void
pvpanicfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	if (ctlr->sem)
		semDelete(ctlr->sem);

	if (ctlr->res)
		vxbResourceFree(ctlr->dev, ctlr->res);

	if (ctlr->iosdrvnum != ERROR)
		iosDrvRemove(ctlr->iosdrvnum, TRUE);

	vxbMemFree(ctlr);
}

LOCAL void *
pvpanicopen(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	Ctlr *ctlr;

	ctlr = (Ctlr *)devhdr;
	if (semTake(ctlr->sem, NO_WAIT) != OK)
		return (void *)ERROR;

	return devhdr;

	(void)name;
	(void)flags;
	(void)mode;
}

LOCAL ssize_t
pvpanicread(void *ctx, char *buf, size_t len)
{
	Ctlr *ctlr;

	if (len < 1)
		return 0;

	ctlr = ctx;
	buf[0] = csr8r(ctlr, 0);
	return 1;
}

LOCAL ssize_t
pvpanicwrite(void *ctx, const char *buf, size_t len)
{
	Ctlr *ctlr;

	if (len != 1)
		return 0;

	ctlr = ctx;
	csr8w(ctlr, 0, buf[0]);
	return 1;
}

LOCAL int
pvpanicclose(void *ctx)
{
	Ctlr *ctlr;

	ctlr = ctx;
	semGive(ctlr->sem);

	return OK;
}

LOCAL int
pvpanicprobe(VXB_DEV_ID dev)
{
	VXB_PCI_DEV_MATCH_ENTRY *match;
	return vxbPciDevMatch(dev, pvpanicpci, &match);
}

LOCAL STATUS
pvpanicattach(VXB_DEV_ID dev)
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
	ctlr->iosdrvnum = iosDrvInstall(NULL, NULL, pvpanicopen, pvpanicclose, pvpanicread, pvpanicwrite, NULL);
	if (ctlr->sem == SEM_ID_NULL || ctlr->res == NULL || ctlr->iosdrvnum == ERROR)
		goto error;

	snprintf(name, sizeof(name), "/pvpanic/%d", vxbDevUnitGet(dev));
	if (iosDevAdd(&ctlr->devhdr, name, ctlr->iosdrvnum) == ERROR)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual;
	ctlr->reghandle = resadr->pHandle;

	return OK;

error:
	pvpanicfree(ctlr);
	return ERROR;
}

LOCAL STATUS
pvpanicdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	pvpanicfree(ctlr);
	return OK;
}

LOCAL STATUS
pvpanicshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD pvpanicdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), pvpanicprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), pvpanicattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), pvpanicdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), pvpanicshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV pvpanicdrv = {
	{ NULL },
	"pvpanic",
	"QEMU pvpanic driver",
	VXB_BUSID_PCI,
	0,
	0,
	pvpanicdev,
	NULL,
};

VXB_DRV_DEF(pvpanicdrv)

STATUS
pvpanicdrvadd(void)
{
	return vxbDrvAdd(&pvpanicdrv);
}

void
pvpanicdrvtest(void)
{
	char buf[1];
	int fd;

	fd = open("/pvpanic/0", O_RDWR);
	if (fd < 0) {
		printf("failed to open device: %s\n", strerror(errno));
		return;
	}

	if (read(fd, buf, sizeof(buf)) != 1)
		printf("failed to read\n");

	printf("events: %#x\n", buf[0]);

	if (write(fd, buf, sizeof(buf)) != 1)
		printf("failed to write\n");

	close(fd);
}
