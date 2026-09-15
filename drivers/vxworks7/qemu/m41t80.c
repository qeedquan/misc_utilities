/*

M41T80 RTC

DTS:

This entry needs to go under a I2C controller block so it can use the I2C controller as a parent for transfers

m41t80@50 {
    compatible = "m41t80";

    // I2C address
    reg = <0x50>;
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

LOCAL const VXB_FDT_DEV_MATCH_ENTRY m41t80fdt[] = {
	{ "m41t80", NULL },
	{ NULL },
};

LOCAL uint8_t
bcd2dec(uint8_t val)
{
	return ((val >> 4) * 10) + (val & 0x0f);
}

LOCAL void
m41t80free(Ctlr *ctlr)
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
m41t80open(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	return devhdr;

	(void)name;
	(void)flags;
	(void)mode;
}

LOCAL ssize_t
m41t80io(void *ctx, int op, void *buf, size_t len)
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
m41t80read(void *ctx, char *buf, size_t len)
{
	return m41t80io(ctx, 'r', buf, len);
}

LOCAL ssize_t
m41t80write(void *ctx, const char *buf, size_t len)
{
	return m41t80io(ctx, 'w', (void *)buf, len);
}

LOCAL int
m41t80close(void *ctx)
{
	return OK;

	(void)ctx;
}

LOCAL int
m41t80probe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, m41t80fdt, NULL);
}

LOCAL STATUS
m41t80attach(VXB_DEV_ID dev)
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
	ctlr->iosdrvnum = iosDrvInstall(NULL, NULL, m41t80open, m41t80close, m41t80read, m41t80write, NULL);
	if (ctlr->sem == SEM_ID_NULL)
		goto error;

	snprintf(name, sizeof(name), "/m41t80/%d", vxbDevUnitGet(dev));
	if (iosDevAdd(&ctlr->devhdr, name, ctlr->iosdrvnum) == ERROR)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	m41t80free(ctlr);
	return ERROR;
}

LOCAL STATUS
m41t80detach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	m41t80free(ctlr);
	return OK;
}

LOCAL STATUS
m41t80shutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

int
mt1480time(const char *name, struct tm *tm)
{
	uint8_t buf[8];
	uint8_t cmd;
	size_t i;
	int fd;
	int r;

	memset(tm, 0, sizeof(*tm));

	r = 0;
	fd = open(name, O_RDWR);
	if (fd < 0)
		return -errno;

	cmd = 0;
	if (write(fd, &cmd, sizeof(cmd)) != sizeof(cmd))
		goto error;
	if (read(fd, buf, sizeof(buf)) != sizeof(buf))
		goto error;

	for (i = 0; i < sizeof(buf); i++)
		buf[i] = bcd2dec(buf[i]);

	tm->tm_sec = buf[1];
	tm->tm_min = buf[2];
	tm->tm_hour = buf[3];
	tm->tm_wday = buf[4];
	tm->tm_mday = buf[5];
	tm->tm_mon = buf[6] - 1;
	tm->tm_year = 100 + buf[7];

	if (0) {
	error:
		r = -EIO;
	}

	close(fd);
	return r;
}

LOCAL VXB_DRV_METHOD m41t80dev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), m41t80probe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), m41t80attach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), m41t80detach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), m41t80shutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV m41t80drv = {
	{ NULL },
	"m41t80",
	"QEMU M41T80 RTC driver",
	VXB_BUSID_FDT,
	0,
	0,
	m41t80dev,
	NULL,
};

VXB_DRV_DEF(m41t80drv)

STATUS
m41t80drvadd(void)
{
	return vxbDrvAdd(&m41t80drv);
}

void
m41t80drvtest(void)
{
	struct tm tm;
	char str[64], *p;

	if (mt1480time("/m41t80/0", &tm) < 0) {
		printf("failed to get time\n");
		return;
	}

	asctime_r(&tm, str);
	if ((p = strchr(str, '\n')))
		*p = '\0';
	printf("%s (UTC)\n", str);
}
