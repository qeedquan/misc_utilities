/*

I2C echo device

DTS:

This needs to go under the I2C controller block so it can use it as a parent for transfers

i2cecho@70 {
    compatible = "i2cecho";

    // I2C address
    reg = <0x70>;
};

*/
#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>

typedef struct {
	DEV_HDR devhdr;
	VXB_DEV_ID dev;
	int addr;

	VXB_RESOURCE *res;
	int iosdrvnum;

	SEM_ID sem;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY i2cechofdt[] = {
	{ "i2cecho", NULL },
	{ NULL },
};

LOCAL void
i2cechofree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	if (ctlr->sem)
		semDelete(ctlr->sem);

	if (ctlr->iosdrvnum != ERROR)
		iosDrvRemove(ctlr->iosdrvnum, TRUE);

	vxbMemFree(ctlr);
}

LOCAL void *
i2cechoopen(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	return devhdr;

	(void)name;
	(void)flags;
	(void)mode;
}

LOCAL ssize_t
i2cechoio(void *ctx, int op, void *buf, size_t len)
{
	Ctlr *ctlr;
	I2C_MSG msg;
	ssize_t r;

	ctlr = ctx;
	memset(&msg, 0, sizeof(msg));
	msg.addr = ctlr->addr;
	msg.buf = buf;
	msg.len = len;
	msg.flags = (op == 'r') ? I2C_M_RD : I2C_M_WR;

	r = len;
	semTake(ctlr->sem, WAIT_FOREVER);
	if (vxbI2cDevXfer(ctlr->dev, &msg, 1) != OK)
		r = -EIO;
	semGive(ctlr->sem);

	return r;
}

LOCAL ssize_t
i2cechoread(void *ctx, char *buf, size_t len)
{
	return i2cechoio(ctx, 'r', buf, len);
}

LOCAL ssize_t
i2cechowrite(void *ctx, const char *buf, size_t len)
{
	return i2cechoio(ctx, 'w', (void *)buf, len);
}

LOCAL int
i2cechoclose(void *ctx)
{
	return OK;

	(void)ctx;
}

LOCAL int
i2cechoprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, i2cechofdt, NULL);
}

LOCAL STATUS
i2cechoattach(VXB_DEV_ID dev)
{
	VXB_FDT_DEV *fdtdev;
	Ctlr *ctlr;
	char name[MAX_DRV_NAME_LEN];
	const UINT32 *prop;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->iosdrvnum = ERROR;

	fdtdev = vxbFdtDevGet(dev);
	if (!fdtdev)
		goto error;

	prop = vxFdtPropGet(fdtdev->offset, "reg", NULL);
	if (!prop)
		goto error;

	ctlr->dev = dev;
	ctlr->addr = vxFdt32ToCpu(*prop);
	ctlr->sem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	ctlr->iosdrvnum = iosDrvInstall(NULL, NULL, i2cechoopen, i2cechoclose, i2cechoread, i2cechowrite, NULL);
	if (ctlr->sem == SEM_ID_NULL)
		goto error;

	snprintf(name, sizeof(name), "/i2cecho/%d", vxbDevUnitGet(dev));
	if (iosDevAdd(&ctlr->devhdr, name, ctlr->iosdrvnum) == ERROR)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	i2cechofree(ctlr);
	return ERROR;
}

LOCAL STATUS
i2cechodetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	i2cechofree(ctlr);
	return OK;
}

LOCAL STATUS
i2cechoshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD i2cechodev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), i2cechoprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), i2cechoattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), i2cechodetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), i2cechoshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV i2cechodrv = {
	{ NULL },
	"i2cecho",
	"QEMU I2C Echo driver",
	VXB_BUSID_FDT,
	0,
	0,
	i2cechodev,
	NULL,
};

VXB_DRV_DEF(i2cechodrv)

STATUS
i2cechodrvadd(void)
{
	return vxbDrvAdd(&i2cechodrv);
}

void
i2cechodrvtest(void)
{
	uint8_t in, out;
	int mismatch;
	int fd;
	int i;

	fd = open("/i2cecho/0", O_RDWR);
	if (fd < 0) {
		printf("open: %s\n", strerror(errno));
		return;
	}

	mismatch = 0;
	for (i = 0x00; i <= 0xff; i++) {
		in = i;
		out = ~i;

		if (write(fd, &in, 1) != 1)
			printf("write: %s\n", strerror(errno));

		if (read(fd, &out, 1) != 1)
			printf("read: %s\n", strerror(errno));

		if (in != out) {
			printf("mismatched: %x %x\n", in, out);
			mismatch++;
		}
	}
	printf("number of mismatches: %d\n", mismatch);

	close(fd);
}
