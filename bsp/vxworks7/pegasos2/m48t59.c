#include "dat.h"
#include "fns.h"

enum {
	REG_ALARM_SEC = 0x1FF2,
	REG_ALARM_MIN = 0x1FF3,
	REG_ALARM_HOUR = 0x1FF4,
	REG_ALARM_MDAY = 0x1FF5,
	REG_INT = 0x1FF6,
	REG_WDT = 0x1FF7,
	REG_CTRL = 0x1FF8,
	REG_SEC = 0x1FF9,
	REG_MIN = 0x1FFA,
	REG_HOUR = 0x1FFB,
	REG_WDAY = 0x1FFC,
	REG_MDAY = 0x1FFD,
	REG_MON = 0x1FFE,
	REG_YEAR = 0x1FFF,
};

enum {
	MIN_YEAR = 2000,
	MAX_YEAR = 2099,
};

LOCAL int
t2v(int t)
{
	int v;

	if (t < 0x1f)
		v = (t << 2) | 0x2;
	else
		v = ((t >> 2) << 2) | 0x3;
	v |= 0x80;
	return v;
}

LOCAL UINT8
tobcd(UINT8 val)
{
	return ((val / 10) << 4) | (val % 10);
}

LOCAL UINT8
frombcd(UINT8 val)
{
	return ((val >> 4) * 10) + (val & 0x0f);
}

LOCAL void
csradrw(M48T59_CTLR *ctlr, UINT16 addr)
{
	vxbWrite8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + 0), addr & 0xff);
	vxbWrite8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + 1), (addr >> 8) & 0xff);
}

LOCAL UINT8
csr8r(M48T59_CTLR *ctlr, UINT16 addr)
{
	csradrw(ctlr, addr);
	return vxbRead8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + 3));
}

LOCAL void
csr8w(M48T59_CTLR *ctlr, UINT16 addr, UINT8 val)
{
	csradrw(ctlr, addr);
	vxbWrite8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + 3), val);
}

STATUS
m48t59rtcget(VXB_DEV_ID dev, struct tm *rtctime)
{
	M48T59_CTLR *ctlr;

	ctlr = vxbDevSoftcGet(dev);

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	rtctime->tm_sec = frombcd(csr8r(ctlr, REG_SEC));
	rtctime->tm_min = frombcd(csr8r(ctlr, REG_MIN));
	rtctime->tm_hour = frombcd(csr8r(ctlr, REG_HOUR));
	rtctime->tm_wday = csr8r(ctlr, REG_WDAY);
	rtctime->tm_mday = frombcd(csr8r(ctlr, REG_MDAY));
	rtctime->tm_mon = frombcd(csr8r(ctlr, REG_MON) - 1);
	rtctime->tm_year = frombcd(csr8r(ctlr, REG_YEAR)) - 1900 + MIN_YEAR;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);
	return OK;
}

STATUS
m48t59rtcset(VXB_DEV_ID dev, struct tm *rtctime)
{
	M48T59_CTLR *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	csr8w(ctlr, REG_SEC, tobcd(rtctime->tm_sec));
	csr8w(ctlr, REG_MIN, tobcd(rtctime->tm_min));
	csr8w(ctlr, REG_HOUR, tobcd(rtctime->tm_hour));
	csr8w(ctlr, REG_WDAY, tobcd(rtctime->tm_wday));
	csr8w(ctlr, REG_MDAY, tobcd(rtctime->tm_mday));
	csr8w(ctlr, REG_MON, tobcd(rtctime->tm_mon + 1));
	csr8w(ctlr, REG_YEAR, tobcd(rtctime->tm_year - MIN_YEAR + 1900));
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);
	return OK;
}

LOCAL STATUS
m48t59alarmget(VXB_DEV_ID dev, UINT8 unit, struct tm *alarm)
{
	M48T59_CTLR *ctlr;

	if (unit > 0)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	memset(alarm, 0, sizeof(*alarm));
	alarm->tm_sec = frombcd(csr8r(ctlr, REG_ALARM_SEC));
	alarm->tm_min = frombcd(csr8r(ctlr, REG_ALARM_MIN));
	alarm->tm_hour = frombcd(csr8r(ctlr, REG_ALARM_HOUR));
	alarm->tm_mday = frombcd(csr8r(ctlr, REG_ALARM_MDAY));
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);
	return OK;
}

LOCAL STATUS
m48t59alarmset(VXB_DEV_ID dev, UINT8 unit, struct tm *alarm, RTC_ALARM_FUNC alarmfunc, void *alarmarg)
{
	if (unit > 0)
		return ERROR;
	return ERROR;
}

STATUS
m48t59nvramget(VXB_DEV_ID dev, char *string, int length, int offset)
{
	M48T59_CTLR *ctlr;
	int size;
	int bytes;

	if (!dev)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	if (!ctlr)
		return ERROR;

	size = ctlr->nvramsize;
	if (offset < 0 || length < 0 || offset > size || length > size || offset + length > size)
		return ERROR;

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	for (bytes = 0; bytes < length; bytes++)
		string[bytes] = csr8r(ctlr, offset + bytes);
	string[bytes] = 0;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

STATUS
m48t59nvramset(VXB_DEV_ID dev, char *string, int length, int offset)
{
	M48T59_CTLR *ctlr;
	int size;
	int bytes;

	if (!dev)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	if (!ctlr)
		return ERROR;

	size = ctlr->nvramsize;
	if (offset < 0 || length < 0 || offset > size || length > size || offset + length > size)
		return ERROR;

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	for (bytes = 0; bytes < length; bytes++)
		csr8w(ctlr, offset + bytes, string[bytes]);
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

LOCAL STATUS
m48t59wdopen(VXB_DEV_ID dev)
{
	M48T59_CTLR *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	csr8w(ctlr, REG_WDT, t2v(ctlr->wdtimeout));
	return OK;
}

LOCAL STATUS
m48t59wdclose(VXB_DEV_ID dev)
{
	M48T59_CTLR *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	csr8w(ctlr, REG_WDT, 0);
	return OK;
}

LOCAL STATUS
m48t59wdread(VXB_DEV_ID dev, UINT8 *buf, UINT32 len, UINT32 *realbyte)
{
	M48T59_CTLR *ctlr;

	if (len == 0)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	csr8r(ctlr, REG_WDT);
	return OK;

	(void)buf;
	(void)len;
	(void)realbyte;
}

LOCAL STATUS
m48t59wdwrite(VXB_DEV_ID dev, UINT8 *buf, UINT32 len, UINT32 *realbyte)
{
	M48T59_CTLR *ctlr;
	UINT32 i;

	ctlr = vxbDevSoftcGet(dev);
	csr8w(ctlr, REG_WDT, t2v(ctlr->wdtimeout));
	for (i = 0; i < len; i++) {
		if (buf[i] == 'V') {
			csr8w(ctlr, REG_WDT, 0);
			break;
		}
	}

	*realbyte = len;
	return OK;
}

LOCAL STATUS
m48t59wdioctl(VXB_DEV_ID dev, int req, _Vx_ioctl_arg_t arg)
{
	M48T59_CTLR *ctlr;
	STATUS status;
	int timeout, flags, options;

	status = OK;
	flags = 0;
	ctlr = vxbDevSoftcGet(dev);

	switch (req) {
	case WDIOC_GETTIMEOUT:
		*(int *)arg = ctlr->wdtimeout;
		break;

	case WDIOC_SETTIMEOUT:
		ctlr->wdtimeout = *(int *)arg;
		ctlr->wdtimeout = min(max(ctlr->wdtimeout, 2), 4 * 0x1f);
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
	timeout = ctlr->wdtimeout;

	if (flags & 0x1)
		csr8w(ctlr, REG_WDT, t2v(timeout));
	else if (flags & 0x2)
		csr8w(ctlr, REG_WDT, 0);

	return status;
}

STATUS
m48t59create(M48T59_CTLR *ctlr)
{
	static VXB_WATCHDOG_INFO wdi = {
		.wdOpen = m48t59wdopen,
		.wdClose = m48t59wdclose,
		.wdRead = m48t59wdread,
		.wdWrite = m48t59wdwrite,
		.wdIoctl = m48t59wdioctl,
	};

	VXB_I2C_RTC_FUNC *rf;
	STATUS status;

	status = OK;

	SPIN_LOCK_ISR_INIT(&ctlr->lock, 0);

	ctlr->wdunit = watchdogDrvNextUnit();
	ctlr->wdtimeout = 2;
	snprintf(ctlr->wdname, sizeof(ctlr->wdname), "/%s/%d", WATCHDOG_DEVICE, ctlr->wdunit);
	if (watchdogDevCreate(ctlr->wdname, ctlr->dev, &wdi) != OK)
		status = ERROR;

	rf = &ctlr->rtcfunc;
	rf->rtcGet = m48t59rtcget;
	rf->rtcSet = m48t59rtcset;
	rf->alarmGet = m48t59alarmget;
	rf->alarmSet = m48t59alarmset;

	rf->minYear = MIN_YEAR - TM_YEAR_BASE;
	rf->maxYear = MAX_YEAR - TM_YEAR_BASE;
	rf->rtcCap = RTC_NEED_PRE_VERIFY;
	rf->alarmNum = 1;
	rf->alarmCap[0] = RTC_ALARM_SEC_SUPPORT;

	ctlr->nvramsize = 0x2000;

	return status;
}

LOCAL VXB_DRV_METHOD m48t59dev[] = {
	VXB_DEVMETHOD_END,
};

VXB_DRV m48t59drv = {
	{ NULL },
	"m48t59",
	"M48T59 RTC/WDT/NVRAM Driver",
	VXB_BUSID_FDT,
	0,
	0,
	m48t59dev,
	NULL,
};

VXB_DRV_DEF(m48t59drv)

void
m48t59link(void)
{
}
