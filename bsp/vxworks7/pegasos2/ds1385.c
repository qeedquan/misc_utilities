#include "dat.h"
#include "fns.h"

// Registers
enum {
	RTC_SECONDS = 0,
	RTC_SECONDS_ALARM = 1,
	RTC_MINUTES = 2,
	RTC_MINUTES_ALARM = 3,
	RTC_HOURS = 4,
	RTC_HOURS_ALARM = 5,

	RTC_DAY_OF_WEEK = 6,
	RTC_DAY_OF_MONTH = 7,
	RTC_MONTH = 8,
	RTC_YEAR = 9,

	REG_A = 10,
	REG_B = 11,
	REG_C = 12,
	REG_D = 13,
};

// Status Bits
enum {
	REG_A_UIP = 0x80,
	REG_A_DV_BITS = 0x70,
	REG_A_DV_4MHZ = 0x00,
	REG_A_DV_1MHZ = 0x10,
	REG_A_DV_32KHZ = 0x40,
	REG_A_RS_BITS = 0x0f,
};

enum {
	REG_B_SET = 0x80,
	REG_B_PIE = 0x40,
	REG_B_AIE = 0x20,
	REG_B_UIE = 0x10,
	REG_B_SQWE = 0x08,
	REG_B_DM = 0x04,
	REG_B_24H = 0x02,
};

enum {
	REG_C_IRQF = 0x80,
	REG_C_PF = 0x40,
	REG_C_AF = 0x20,
	REG_C_UF = 0x10,
};

enum {
	MIN_YEAR = 2000,
	MAX_YEAR = 2099,
};

LOCAL UINT8
csr8r(DS1385_CTLR *ctlr, UINT8 addr)
{
	vxbWrite8(ctlr->reghandle, (UINT8 *)ctlr->regbase, addr & 0xff);
	return vxbRead8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + 1));
}

LOCAL void
csr8w(DS1385_CTLR *ctlr, UINT8 addr, UINT8 value)
{
	vxbWrite8(ctlr->reghandle, (UINT8 *)ctlr->regbase, addr & 0xff);
	return vxbWrite8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + 1), value);
}

LOCAL STATUS
readrtc(VXB_DEV_ID dev, struct tm *tm, int type)
{
	DS1385_CTLR *ctlr;
	UINT8 data;
	int retries;
	STATUS status;

	if (dev == NULL || tm == NULL || !(0 <= type && type <= 1))
		return ERROR;

	status = OK;
	ctlr = vxbDevSoftcGet(dev);
	retries = 5;

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);

	if (!(csr8r(ctlr, REG_B) & REG_B_DM))
		goto error;

	while (retries--) {
		data = csr8r(ctlr, REG_A);
		if (!(data & REG_A_UIP))
			break;
		vxbUsDelay(10);
	}
	if (retries <= 0)
		goto error;

	memset(tm, 0, sizeof(*tm));
	tm->tm_sec = csr8r(ctlr, RTC_SECONDS);
	tm->tm_min = csr8r(ctlr, RTC_MINUTES);
	tm->tm_hour = csr8r(ctlr, RTC_HOURS);
	tm->tm_wday = csr8r(ctlr, RTC_DAY_OF_WEEK) - 1;
	tm->tm_mday = csr8r(ctlr, RTC_DAY_OF_MONTH);
	tm->tm_mon = csr8r(ctlr, RTC_MONTH) - 1;
	tm->tm_year = csr8r(ctlr, RTC_YEAR) + 100;
	if (type == 1) {
		tm->tm_sec = csr8r(ctlr, RTC_SECONDS_ALARM);
		tm->tm_min = csr8r(ctlr, RTC_MINUTES_ALARM);
		tm->tm_hour = csr8r(ctlr, RTC_HOURS_ALARM);
	}

	if (0) {
	error:
		status = ERROR;
	}
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return status;
}

LOCAL STATUS
writertc(VXB_DEV_ID dev, struct tm *tm, int type, RTC_ALARM_FUNC alarmfunc, void *alarmarg)
{
	DS1385_CTLR *ctlr;
	STATUS status;
	UINT8 data;

	if (dev == NULL || tm == NULL || !(0 <= type && type <= 1))
		return ERROR;

	status = OK;
	ctlr = vxbDevSoftcGet(dev);

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	if (!(csr8r(ctlr, REG_B) & REG_B_DM))
		goto error;

	if (type == 0) {
		// abort update cycle
		data = csr8r(ctlr, REG_B) | REG_B_SET;
		csr8w(ctlr, REG_B, data);

		csr8w(ctlr, RTC_SECONDS, tm->tm_sec & 0xff);
		csr8w(ctlr, RTC_MINUTES, tm->tm_min & 0xff);
		csr8w(ctlr, RTC_HOURS, tm->tm_hour & 0xff);
		csr8w(ctlr, RTC_DAY_OF_WEEK, (tm->tm_wday + 1) & 0xff);
		csr8w(ctlr, RTC_DAY_OF_MONTH, tm->tm_mday & 0xff);
		csr8w(ctlr, RTC_MONTH, (tm->tm_mon + 1) & 0xff);
		csr8w(ctlr, RTC_YEAR, (tm->tm_year - 100) & 0xff);

		// resume update cycle
		data &= ~REG_B_SET;
		csr8w(ctlr, REG_B, data);
	} else {
		ctlr->alarmfunc = alarmfunc;
		ctlr->alarmarg = alarmarg;

		// disable interrupt alarm
		data = csr8r(ctlr, REG_B);
		data &= ~REG_B_AIE;
		csr8w(ctlr, REG_B, data);

		// clear interrupt flag
		ctlr->savedint = csr8r(ctlr, REG_C);
		ctlr->savedint &= REG_C_IRQF | REG_C_PF;

		csr8w(ctlr, RTC_SECONDS_ALARM, tm->tm_sec & 0xff);
		csr8w(ctlr, RTC_MINUTES_ALARM, tm->tm_min & 0xff);
		csr8w(ctlr, RTC_HOURS_ALARM, tm->tm_hour & 0xff);

		// enable interrupt alarm
		data |= REG_B_AIE;
		csr8w(ctlr, REG_B, data);
	}

	if (0) {
	error:
		status = ERROR;
	}

	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return status;
}

void
ds1385int(VXB_DEV_ID dev)
{
	DS1385_CTLR *ctlr;

	void (*isrfunc)(_Vx_usr_arg_t);
	_Vx_usr_arg_t israrg;

	RTC_ALARM_FUNC alarmfunc;
	void *alarmarg;

	BOOL timerenabled;
	UINT8 data;

	ctlr = vxbDevSoftcGet(dev);

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);

	data = csr8r(ctlr, REG_C) | ctlr->savedint;
	ctlr->savedint = 0;

	timerenabled = ctlr->timerenabled;
	isrfunc = ctlr->isrfunc;
	israrg = ctlr->israrg;

	alarmfunc = ctlr->alarmfunc;
	alarmarg = ctlr->alarmarg;

	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	if (data & REG_C_IRQF) {
		if (timerenabled && (data & REG_C_PF) && isrfunc)
			isrfunc(israrg);

		if ((data & REG_C_AF) && alarmfunc)
			alarmfunc(alarmarg);
	}
}

LOCAL STATUS
ds1385rtcget(VXB_DEV_ID dev, struct tm *time)
{
	return readrtc(dev, time, 0);
}

LOCAL STATUS
ds1385rtcset(VXB_DEV_ID dev, struct tm *time)
{
	return writertc(dev, time, 0, NULL, NULL);
}

LOCAL STATUS
ds1385alarmget(VXB_DEV_ID dev, UINT8 unit, struct tm *alarm)
{
	if (unit > 0)
		return ERROR;
	return readrtc(dev, alarm, 1);
}

LOCAL STATUS
ds1385alarmset(VXB_DEV_ID dev, UINT8 unit, struct tm *alarm, RTC_ALARM_FUNC alarmfunc, void *alarmarg)
{
	if (unit > 0)
		return ERROR;
	return writertc(dev, alarm, 1, alarmfunc, alarmarg);
}

LOCAL STATUS
ds1385disable(void *cookie)
{
	DS1385_CTLR *ctlr;
	UINT8 val;

	if (cookie == NULL)
		return ERROR;

	ctlr = cookie;
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	if (!ctlr->timerenabled)
		goto out;

	// disable periodic timer interrupts
	val = csr8r(ctlr, REG_B) & ~REG_B_PIE;
	csr8w(ctlr, REG_B, val);

	// set time-base frequency to 32768
	// disable square wave/periodic output
	csr8w(ctlr, REG_A, REG_A_DV_32KHZ);

	// clear the interrupt flag
	ctlr->savedint = csr8r(ctlr, REG_C) & (REG_C_IRQF | REG_C_AF);

	ctlr->timerenabled = FALSE;

out:
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);
	return OK;
}

LOCAL STATUS
ds1385enable(void *cookie, UINT32 maxtimercount)
{
	static const struct {
		UINT32 rate;
		UINT8 bits;
	} freqs[] = {
		{ 2, 0x0f },
		{ 4, 0x0e },
		{ 8, 0x0d },
		{ 16, 0x0c },
		{ 32, 0x0b },
		{ 64, 0x0a },
		{ 128, 0x09 },
		{ 256, 0x08 },
		{ 512, 0x07 },
		{ 1024, 0x06 },
		{ 2048, 0x05 },
		{ 4096, 0x04 },
		{ 8192, 0x03 },
	};

	DS1385_CTLR *ctlr;
	STATUS status;
	UINT32 desired;
	UINT8 val;
	size_t i;

	if (cookie == NULL)
		return ERROR;

	ctlr = cookie;
	status = OK;
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	if (ctlr->timerenabled)
		goto out;

	desired = ctlr->timerfunc.clkFrequency / maxtimercount;
	for (i = 0; i < NELEMENTS(freqs); i++) {
		if (freqs[i].rate >= desired) {
			if (freqs[i].rate > desired && i > 0)
				i--;

			// update ticks per second
			val = REG_A_DV_32KHZ | freqs[i].bits;
			ctlr->timerfunc.ticksPerSecond = freqs[i].rate;
			csr8w(ctlr, REG_A, val);

			ctlr->timerenabled = TRUE;

			// enable periodic interrupt
			val = csr8r(ctlr, REG_B) | REG_B_PIE;
			csr8w(ctlr, REG_B, val);
			break;
		}
	}
	if (i == NELEMENTS(freqs))
		status = ERROR;

out:
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);
	return status;
}

LOCAL STATUS
ds1385allocate(void *cookie, UINT32 flags)
{
	DS1385_CTLR *ctlr;
	STATUS status;

	if (cookie == NULL || !(flags & VXB_TIMER_AUTO_RELOAD))
		return ERROR;

	status = OK;
	ctlr = cookie;

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	if (ctlr->timerfunc.allocated)
		status = ERROR;
	else
		ctlr->timerfunc.allocated = TRUE;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return status;
}

LOCAL STATUS
ds1385release(void *cookie)
{
	DS1385_CTLR *ctlr;

	if (!cookie)
		return ERROR;

	ctlr = cookie;
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	if (!ctlr->timerfunc.allocated) {
		SPIN_LOCK_ISR_GIVE(&ctlr->lock);
		return ERROR;
	}

	ds1385disable(cookie);
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	ctlr->isrfunc = NULL;
	ctlr->israrg = 0;
	ctlr->timerfunc.allocated = FALSE;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

LOCAL STATUS
ds1385rolloverget(void *cookie, UINT32 *count)
{
	return ERROR;
}

LOCAL STATUS
ds1385countget(void *cookie, UINT32 *count)
{
	return ERROR;
}

LOCAL STATUS
ds1385isrset(void *cookie, void (*func)(_Vx_usr_arg_t), _Vx_usr_arg_t arg)
{
	DS1385_CTLR *ctlr;

	if (!cookie)
		return ERROR;

	ctlr = cookie;
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	ctlr->isrfunc = func;
	ctlr->israrg = arg;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

STATUS
ds1385create(DS1385_CTLR *ctlr, int unit, UINT32 minfreq, UINT32 maxfreq, UINT32 clkfreq)
{
	struct vxbTimerFunctionality *tf;
	VXB_I2C_RTC_FUNC *rf;
	UINT8 data;

	SPIN_LOCK_ISR_INIT(&ctlr->lock, 0);

	tf = &ctlr->timerfunc;
	tf->timerNo = unit;
	tf->minFrequency = minfreq;
	tf->maxFrequency = maxfreq;
	tf->clkFrequency = clkfreq;
	tf->ticksPerSecond = 64;
	tf->features = VXB_TIMER_CAN_INTERRUPT |
	               VXB_TIMER_SIZE_32 |
	               VXB_TIMER_AUTO_RELOAD |
	               VXB_TIMER_CANNOT_SUPPORT_ALL_FREQS;
	tf->timerAllocate = ds1385allocate;
	tf->timerRelease = ds1385release;
	tf->timerRolloverGet = ds1385rolloverget;
	tf->timerCountGet = ds1385countget;
	tf->timerDisable = ds1385disable;
	tf->timerEnable = ds1385enable;
	tf->timerISRSet = ds1385isrset;
	snprintf(tf->timerName, sizeof(tf->timerName), "ds1385-timer-%d", tf->timerNo);
	vxbTimerRegister(tf);

	rf = &ctlr->rtcfunc;
	rf->rtcGet = ds1385rtcget;
	rf->rtcSet = ds1385rtcset;
	rf->alarmSet = ds1385alarmset;
	rf->alarmGet = ds1385alarmget;

	rf->alarmNum = 1;
	rf->alarmCap[0] = RTC_ALARM_SEC_SUPPORT;
	rf->minYear = MIN_YEAR - TM_YEAR_BASE;
	rf->maxYear = MAX_YEAR - TM_YEAR_BASE;
	rf->rtcCap = RTC_NEED_PRE_VERIFY;

	rtcRegister(ctlr->dev, rf);

	// use date mode (disable bcd encoding) and use 24 hour time
	data = csr8r(ctlr, REG_B) | REG_B_DM | REG_B_24H;
	csr8w(ctlr, REG_B, data);

	return OK;
}

LOCAL VXB_DRV_METHOD ds1385dev[] = {
	VXB_DEVMETHOD_END,
};

VXB_DRV ds1385drv = {
	{ NULL },
	"ds1385",
	"DS1385 Timer/RTC Driver",
	VXB_BUSID_FDT,
	0,
	0,
	ds1385dev,
	NULL,
};

VXB_DRV_DEF(ds1385drv)

void
ds1385link(void)
{
}