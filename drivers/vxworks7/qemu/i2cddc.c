/*

DTS:

This needs to go under the I2C controller block so it can use it as a parent for transfers

i2cddc@30 {
    compatible = "i2cddc";

    // I2C address
    reg = <0x30>;
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

LOCAL const VXB_FDT_DEV_MATCH_ENTRY i2cddcfdt[] = {
	{ "i2cddc", NULL },
	{ NULL },
};

LOCAL void
i2cddcfree(Ctlr *ctlr)
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
i2cddcopen(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	return devhdr;

	(void)name;
	(void)flags;
	(void)mode;
}

LOCAL ssize_t
i2cddcio(void *ctx, int op, void *buf, size_t len)
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
i2cddcread(void *ctx, char *buf, size_t len)
{
	return i2cddcio(ctx, 'r', buf, len);
}

LOCAL ssize_t
i2cddcwrite(void *ctx, const char *buf, size_t len)
{
	return i2cddcio(ctx, 'w', (void *)buf, len);
}

LOCAL int
i2cddcclose(void *ctx)
{
	return OK;

	(void)ctx;
}

LOCAL int
i2cddcprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, i2cddcfdt, NULL);
}

LOCAL STATUS
i2cddcattach(VXB_DEV_ID dev)
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
	ctlr->iosdrvnum = iosDrvInstall(NULL, NULL, i2cddcopen, i2cddcclose, i2cddcread, i2cddcwrite, NULL);
	if (ctlr->sem == SEM_ID_NULL)
		goto error;

	snprintf(name, sizeof(name), "/i2cddc/%d", vxbDevUnitGet(dev));
	if (iosDevAdd(&ctlr->devhdr, name, ctlr->iosdrvnum) == ERROR)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	i2cddcfree(ctlr);
	return ERROR;
}

LOCAL STATUS
i2cddcdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	i2cddcfree(ctlr);
	return OK;
}

LOCAL STATUS
i2cddcshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

STATUS
i2cddcget(const char *name, uint8_t *buf, size_t len)
{
	uint8_t cmd;
	int fd;
	STATUS r;

	fd = open(name, O_RDONLY);
	if (fd < 0)
		return ERROR;

	r = OK;
	cmd = 0;
	if (write(fd, &cmd, sizeof(cmd)) != sizeof(cmd))
		goto error;
	if (read(fd, buf, len) != (ssize_t)len)
		goto error;

	if (0) {
	error:
		r = ERROR;
	}

	close(fd);
	return r;
}

STATUS
i2cddcshow(const char *name)
{
	uint8_t edid[128];
	STATUS r;

	r = i2cddcget(name, edid, sizeof(edid));
	if (r != OK)
		return r;

	printf("EDID\n");
	printf("Header: %02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
	    edid[0], edid[1], edid[2], edid[3], edid[4], edid[5], edid[6], edid[7]);
	printf("Manufacturer ID: %02x-%02x\n", edid[8], edid[9]);
	printf("Product ID: %02x%02x\n", edid[11], edid[10]);
	printf("Serial Number: %02x-%02x-%-02x-%02x\n", edid[12], edid[13], edid[14], edid[15]);
	printf("Manufacture Date: %d/%d\n", edid[16], 1990 + edid[17]);
	printf("Version: %d\n", edid[18]);
	printf("Revision: %d\n", edid[19]);
	printf("Video Input Type: %d\n", edid[20]);
	printf("Horizontal Size (cm): %d\n", edid[21]);
	printf("Vertical Size (cm): %d\n", edid[22]);
	return r;
}

LOCAL VXB_DRV_METHOD i2cddcdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), i2cddcprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), i2cddcattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), i2cddcdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), i2cddcshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV i2cddcdrv = {
	{ NULL },
	"i2cddc",
	"QEMU I2C DDC driver",
	VXB_BUSID_FDT,
	0,
	0,
	i2cddcdev,
	NULL,
};

VXB_DRV_DEF(i2cddcdrv)

STATUS
i2cddcdrvadd(void)
{
	return vxbDrvAdd(&i2cddcdrv);
}
