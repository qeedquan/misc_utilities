/*

Watchdog driver for ib700 device, this is hardwired to a fixed IO port, so only one entry should be inside the DTS file.

DTS:

ib700wdt {
    compatible = "ib700wdt";
    // STOP, START
    io = <0x441 1
          0x443 1>;
    timeout-sec = <30>;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <iosLib.h>
#include <errno.h>
#include <errnoLib.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/drv/watchdog/vxbWatchdog.h>
#include <hwif/drv/watchdog/watchdog.h>
#include <subsys/timer/vxbTimerLib.h>

#define t2v(x) (0xf - (((x) + 1) / 2))

enum {
	STOP,
	START,
	IOREGS,
};

typedef struct {
	DEV_HDR devhdr;
	VXB_DEV_ID dev;
	char devname[MAX_DRV_NAME_LEN];

	int unit;
	int timeout;

	VXB_RESOURCE *iores[IOREGS];
	VIRT_ADDR iobase[IOREGS];
	void *iohandle[IOREGS];
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY ib700wdfdt[] = {
	{ "ib700wdt", NULL },
	{ NULL },
};

LOCAL void
wdio(Ctlr *ctlr, int addr, int value)
{
	vxbWrite8(ctlr->iohandle[addr], (UINT8 *)ctlr->iobase[addr], value);
}

LOCAL void
wdfree(Ctlr *ctlr)
{
	size_t i;

	if (!ctlr)
		return;

	for (i = 0; i < IOREGS; i++) {
		if (ctlr->iores[i])
			vxbResourceFree(ctlr->dev, ctlr->iores[i]);
	}

	vxbMemFree(ctlr);
}

LOCAL STATUS
wdopen(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	wdio(ctlr, START, t2v(ctlr->timeout));
	return OK;
}

LOCAL STATUS
wdclose(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	wdio(ctlr, STOP, 0);
	return OK;
}

LOCAL STATUS
wdread(VXB_DEV_ID dev, UINT8 *buf, UINT32 len, UINT32 *realbyte)
{
	Ctlr *ctlr;

	if (len == 0)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	wdio(ctlr, START, t2v(ctlr->timeout));
	return OK;

	(void)buf;
	(void)len;
	(void)realbyte;
}

LOCAL STATUS
wdwrite(VXB_DEV_ID dev, UINT8 *buf, UINT32 len, UINT32 *realbyte)
{
	Ctlr *ctlr;
	UINT32 i;

	ctlr = vxbDevSoftcGet(dev);
	wdio(ctlr, START, t2v(ctlr->timeout));
	for (i = 0; i < len; i++) {
		if (buf[i] == 'V') {
			wdio(ctlr, STOP, 0);
			break;
		}
	}

	*realbyte = len;
	return OK;
}

LOCAL STATUS
wdioctl(VXB_DEV_ID dev, int req, _Vx_ioctl_arg_t arg)
{
	Ctlr *ctlr;
	STATUS status;
	int timeout, flags, options;

	status = OK;
	flags = 0;
	ctlr = vxbDevSoftcGet(dev);

	switch (req) {
	case WDIOC_GETTIMEOUT:
		*(int *)arg = ctlr->timeout;
		break;

	case WDIOC_SETTIMEOUT:
		ctlr->timeout = *(int *)arg;
		ctlr->timeout = min(max(ctlr->timeout, 1), 30);
		flags |= 0x1;
		break;

	case WDIOC_KEEPALIVE:
		flags |= 0x1;
		break;

	case WDIOC_SETOPTIONS:
		options = *(int *)arg;
		if (options & WDIOS_DISABLECARD)
			flags |= 0x2;
		else if (options & (WDIOS_ENABLECARD | WDIOS_FORCERESET))
			flags |= 0x1;
		else
			status = ERROR;
		break;

	default:
		errnoSet(ENOTSUP);
		status = ERROR;
		break;
	}
	timeout = ctlr->timeout;

	if (flags & 0x1)
		wdio(ctlr, START, t2v(timeout));
	else if (flags & 0x2)
		wdio(ctlr, STOP, 0);

	return status;
}

LOCAL STATUS
wdshutdown(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	wdio(ctlr, STOP, 0);
	return OK;
}

LOCAL STATUS
wdprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, ib700wdfdt, NULL);
}

LOCAL STATUS
wdattach(VXB_DEV_ID dev)
{
	static VXB_WATCHDOG_INFO wdi = {
		.wdOpen = wdopen,
		.wdClose = wdclose,
		.wdRead = wdread,
		.wdWrite = wdwrite,
		.wdIoctl = wdioctl,
	};

	VXB_RESOURCE_ADR *resadr;
	VXB_FDT_DEV *fdtdev;
	Ctlr *ctlr;
	const UINT32 *prop;
	int proplen;
	size_t i;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	fdtdev = vxbFdtDevGet(dev);
	if (!fdtdev)
		goto error;

	ctlr->dev = dev;
	for (i = 0; i < IOREGS; i++) {
		ctlr->iores[i] = vxbResourceAlloc(dev, VXB_RES_IO, i);
		if (!ctlr->iores[i])
			goto error;
		resadr = ctlr->iores[i]->pRes;
		ctlr->iohandle[i] = resadr->pHandle;
		ctlr->iobase[i] = resadr->virtual;
	}
	ctlr->unit = watchdogDrvNextUnit();
	vxbDevSoftcSet(dev, ctlr);

	snprintf(ctlr->devname, sizeof(ctlr->devname), "/%s/%d", WATCHDOG_DEVICE, ctlr->unit);
	if (watchdogDevCreate(ctlr->devname, dev, &wdi) != OK)
		goto error;

	prop = vxFdtPropGet(fdtdev->offset, "timeout-sec", &proplen);
	if (prop && proplen == 4)
		ctlr->timeout = vxFdt32ToCpu(*prop);
	ctlr->timeout = min(max(ctlr->timeout, 1), 30);

	return OK;

error:
	wdfree(ctlr);
	return ERROR;
}

LOCAL STATUS
wddetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	vxbDevSoftcSet(dev, NULL);
	wdfree(ctlr);
	return OK;
}

LOCAL VXB_DRV_METHOD ib700wddev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), wdprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), wdattach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), wdshutdown },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), wddetach },
	VXB_DEVMETHOD_END,
};

VXB_DRV ib700wddrv = {
	{ NULL },
	"ib700wdt",
	"QEMU IB700 Watchdog Driver",
	VXB_BUSID_FDT,
	0,
	0,
	ib700wddev,
	NULL,
};

VXB_DRV_DEF(ib700wddrv)

STATUS
ib700wddrvadd(void)
{
	return vxbDrvAdd(&ib700wddrv);
}

void
ib700wdtest(void)
{
	VXB_DEV_ID dev;
	Ctlr *ctlr;
	int fd;
	int timeout;

	fd = -1;
	dev = vxbDevAcquireByName("ib700wdt", 0);
	if (dev == NULL)
		goto out;

	ctlr = vxbDevSoftcGet(dev);
	printf("device: %s\n", ctlr->devname);

	fd = open(ctlr->devname, O_RDWR);
	if (fd < 0)
		goto out;

	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	printf("timeout: %d\n", timeout);
	timeout = 5;
	ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	printf("new timeout: %d\n", timeout);

	sleep(timeout + 2);

out:
	vxbDevRelease(dev);

	if (fd >= 0)
		close(fd);
}
