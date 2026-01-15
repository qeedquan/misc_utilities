#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <errnoLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <endian.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/drv/watchdog/vxbWatchdog.h>
#include <hwif/drv/watchdog/watchdog.h>
#include <subsys/timer/vxbTimerLib.h>

enum {
	ESB_CONFIG_REG = 0x60,
	ESB_LOCK_REG = 0x68,
};

enum {
	ESB_UNLOCK1 = 0x80,
	ESB_UNLOCK2 = 0x86,
};

enum {
	ESB_MAX_TIMER_VALUE = 0xfffff,
};

typedef struct {
	DEV_HDR devhdr;
	VXB_DEV_ID dev;

	char devname[MAX_DRV_NAME_LEN];
	int unit;

	UINT64 clkrate;
	BOOL enable;

	int mintimeout;
	int maxtimeout;
	int timeout;

	VXB_RESOURCE *res;

	VIRT_ADDR regbase;
	void *reghandle;

	spinlockIsr_t lock;
} Ctlr;

#define csr16r(c, a) vxbRead16(c->reghandle, (UINT16 *)(((char *)(c)->regbase) + ((a))))
#define csr32r(c, a) vxbRead32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))))

#define csr16w(c, a, v) vxbWrite16(c->reghandle, (UINT16 *)(((char *)(c)->regbase) + ((a))), v)
#define csr32w(c, a, v) vxbWrite32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))), v)

LOCAL const VXB_PCI_DEV_MATCH_ENTRY i6300wdpci[] = {
	{ 0x25ab, 0x8086, NULL },
	{},
};

LOCAL const struct watchdog_info i6300wdident = {
	.flags = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_CARDRESET,
	.version = 0,
	.name = "i6300wd",
};

LOCAL void
i6300wdfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbResourceFree(ctlr->dev, ctlr->res);
	vxbMemFree(ctlr);
}

LOCAL void
i6300wdtoggle(VXB_DEV_ID dev, BOOL enable)
{
	Ctlr *ctlr;
	UINT16 bit0, bit1;

	ctlr = vxbDevSoftcGet(dev);

	// enable reboot on timeout, 1khz clock
	bit0 = 0;

	// enable the watchdog
	bit1 = 1 << 1;

	if (!enable) {
		bit0 |= (1 << 5);
		bit1 &= ~(1 << 1);
	}

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	VXB_PCI_BUS_CFG_WRITE(dev, ESB_CONFIG_REG, 2, bit0);
	VXB_PCI_BUS_CFG_WRITE(dev, ESB_LOCK_REG, 1, bit1);
	ctlr->enable = enable;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);
}

LOCAL STATUS
i6300wdsettimeout(VXB_DEV_ID dev, int timeout)
{
	Ctlr *ctlr;
	UINT32 val, pre0, pre1;

	ctlr = vxbDevSoftcGet(dev);
	if (timeout < ctlr->mintimeout || timeout > ctlr->maxtimeout)
		return ERROR;

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	ctlr->timeout = timeout;

	val = timeout * ctlr->clkrate;
	if (val <= ESB_MAX_TIMER_VALUE) {
		pre0 = val;
		pre1 = 0;
	} else {
		pre0 = ESB_MAX_TIMER_VALUE;
		pre1 = val - ESB_MAX_TIMER_VALUE;
	}
	pre0 = max(pre0, 1);
	pre1 = max(pre1, 1);

	csr32w(ctlr, 0xc, ESB_UNLOCK1);
	csr32w(ctlr, 0xc, ESB_UNLOCK2);
	csr32w(ctlr, 0x0, pre0);

	csr32w(ctlr, 0xc, ESB_UNLOCK1);
	csr32w(ctlr, 0xc, ESB_UNLOCK2);
	csr32w(ctlr, 0x4, pre1);
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

LOCAL void
i6300wdreset(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	i6300wdsettimeout(dev, ctlr->mintimeout);
	i6300wdtoggle(dev, FALSE);
	i6300wdtoggle(dev, TRUE);
}

LOCAL void
i6300wdservice(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	csr16w(ctlr, 0xc, ESB_UNLOCK1);
	csr16w(ctlr, 0xc, ESB_UNLOCK2);
	csr16w(ctlr, 0xc, 0x100);
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);
}

LOCAL STATUS
i6300wdopen(VXB_DEV_ID dev)
{
	i6300wdtoggle(dev, TRUE);
	return OK;
}

LOCAL STATUS
i6300wdread(VXB_DEV_ID dev, UINT8 *buf, UINT32 len, UINT32 *realbyte)
{
	if (len == 0)
		return ERROR;

	i6300wdservice(dev);
	return OK;
}

LOCAL STATUS
i6300wdwrite(VXB_DEV_ID dev, UINT8 *buf, UINT32 len, UINT32 *realbyte)
{
	UINT32 i;

	if (len == 0)
		return ERROR;

	for (i = 0; i < len; i++) {
		if (buf[i] == 'V') {
			i6300wdtoggle(dev, FALSE);
			return OK;
		}
	}

	*realbyte = len;
	return OK;
}

LOCAL STATUS
i6300wdioctl(VXB_DEV_ID dev, int req, _Vx_ioctl_arg_t arg)
{
	Ctlr *ctlr;
	STATUS status;
	int options;

	status = OK;
	ctlr = vxbDevSoftcGet(dev);
	switch (req) {
	case WDIOC_GETSUPPORT:
		memcpy((struct watchdog_info *)arg, &i6300wdident, sizeof(i6300wdident));
		break;

	case WDIOC_KEEPALIVE:
		i6300wdservice(dev);
		break;

	case WDIOC_SETOPTIONS:
		options = *(int *)arg;
		if (options & WDIOS_DISABLECARD)
			i6300wdtoggle(dev, FALSE);
		else if (options & WDIOS_ENABLECARD)
			i6300wdtoggle(dev, TRUE);
		else if (options & WDIOS_FORCERESET)
			i6300wdreset(dev);
		else
			status = ERROR;
		break;

	case WDIOC_GETTIMEOUT:
		*(int *)arg = ctlr->timeout;
		break;

	case WDIOC_SETTIMEOUT:
		status = i6300wdsettimeout(dev, *(int *)arg);
		if (ctlr->enable) {
			i6300wdtoggle(dev, FALSE);
			i6300wdtoggle(dev, TRUE);
		}
		break;

	default:
		errnoSet(ENOTSUP);
		status = ERROR;
		break;
	}

	return status;
}

LOCAL STATUS
i6300wdclose(VXB_DEV_ID dev)
{
	i6300wdtoggle(dev, FALSE);
	return OK;
}

LOCAL int
i6300wdprobe(VXB_DEV_ID dev)
{
	return vxbPciDevMatch(dev, i6300wdpci, NULL);
}

LOCAL STATUS
i6300wdattach(VXB_DEV_ID dev)
{
	static VXB_WATCHDOG_INFO wdi = {
		.wdOpen = i6300wdopen,
		.wdClose = i6300wdclose,
		.wdRead = i6300wdread,
		.wdWrite = i6300wdwrite,
		.wdIoctl = i6300wdioctl,
	};

	Ctlr *ctlr;
	VXB_RESOURCE_ADR *resadr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->res = vxbResourceAlloc(dev, VXB_RES_MEMORY, 0);
	if (ctlr->res == NULL)
		goto error;

	ctlr->unit = watchdogDrvNextUnit();
	vxbDevSoftcSet(dev, ctlr);

	snprintf(ctlr->devname, sizeof(ctlr->devname), "/%s/%d", WATCHDOG_DEVICE, ctlr->unit);
	if (watchdogDevCreate(ctlr->devname, dev, &wdi) != OK)
		goto error;

	SPIN_LOCK_ISR_INIT(&ctlr->lock, 0);

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual;
	ctlr->reghandle = resadr->pHandle;

	ctlr->clkrate = 1000;
	ctlr->mintimeout = 1;
	ctlr->maxtimeout = (ESB_MAX_TIMER_VALUE / ctlr->clkrate) * 2;

	i6300wdsettimeout(dev, ctlr->mintimeout);

	return OK;

error:
	i6300wdfree(ctlr);
	return ERROR;
}

LOCAL STATUS
i6300wddetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	i6300wdfree(ctlr);
	vxbDevSoftcSet(dev, NULL);
	return OK;
}

LOCAL STATUS
i6300wdshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD i6300wddev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), i6300wdprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), i6300wdattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), i6300wddetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), i6300wdshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV i6300wddrv = {
	{ NULL },
	"i6300wd",
	"Intel 6300 Watchdog driver",
	VXB_BUSID_PCI,
	0,
	0,
	i6300wddev,
	NULL,
};

VXB_DRV_DEF(i6300wddrv)

STATUS
i6300wddrvadd(void)
{
	return vxbDrvAdd(&i6300wddrv);
}

void
i6300wdlink(void)
{
}
