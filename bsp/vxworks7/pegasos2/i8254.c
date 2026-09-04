#include "dat.h"
#include "fns.h"

#define csr8r(c, a) vxbRead8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))))
#define csr8w(c, a, v) vxbWrite8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))), v)

enum {
	T0cntr = 0x0, // counter ports
	T1cntr = 0x1,
	T2cntr = 0x2,
	Tmode = 0x3, // mode port (control word register)

	Latch0 = 0x0,   // latch counter 0's value
	Latch0l = 0x10, // load counter 0's lsb
	Load0m = 0x20,  // load counter 0's msb
	Load0 = 0x30,   // load counter 0 with 2 bytes
};

LOCAL void
reloadcount(I8254_CTLR *ctlr, UINT32 count)
{
	UINT8 reg;

	reg = Load0 | ((ctlr->selectmode << 1) & 0xff);
	csr8w(ctlr, Tmode, reg);
	csr8w(ctlr, T0cntr, count & 0xff);
	csr8w(ctlr, T0cntr, (count >> 8) & 0xff);
}

LOCAL STATUS
disableint(I8254_CTLR *ctlr)
{
	if (!ctlr->enabled)
		return ERROR;

	ctlr->selectmode = 4;
	ctlr->enabled = FALSE;

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	ctlr->maxcount = 0xffff;
	reloadcount(ctlr, 0);
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

void
i8254int(VXB_DEV_ID dev)
{
	I8254_CTLR *ctlr;
	void (*isrfunc)(_Vx_usr_arg_t);
	_Vx_usr_arg_t israrg;
	BOOL enabled;

	ctlr = vxbDevSoftcGet(dev);
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	enabled = ctlr->enabled;
	isrfunc = ctlr->isrfunc;
	israrg = ctlr->israrg;
	if (ctlr->autoreload)
		reloadcount(ctlr, ctlr->maxcount);
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	if (enabled && isrfunc)
		isrfunc(israrg);
}

LOCAL STATUS
i8254disable(void *cookie)
{
	I8254_CTLR *ctlr;
	STATUS status;

	if (cookie == NULL)
		return ERROR;

	ctlr = cookie;
	semTake(ctlr->sem, WAIT_FOREVER);
	status = disableint(ctlr);
	semGive(ctlr->sem);

	return status;
}

LOCAL STATUS
i8254allocate(void *cookie, UINT32 flags)
{
	I8254_CTLR *ctlr;
	STATUS status;

	if (cookie == NULL)
		return ERROR;

	status = OK;
	ctlr = cookie;
	semTake(ctlr->sem, WAIT_FOREVER);
	if (ctlr->timerfunc.allocated)
		status = ERROR;
	else {
		ctlr->timerfunc.allocated = TRUE;
		if (flags & VXB_TIMER_AUTO_RELOAD)
			ctlr->autoreload = TRUE;
	}
	semGive(ctlr->sem);

	return status;
}

LOCAL STATUS
i8254release(void *cookie)
{
	I8254_CTLR *ctlr;

	if (cookie == NULL)
		return ERROR;

	ctlr = cookie;
	semTake(ctlr->sem, WAIT_FOREVER);
	if (ctlr->timerfunc.allocated) {
		disableint(ctlr);
		ctlr->isrfunc = NULL;
		ctlr->israrg = 0;
		ctlr->autoreload = FALSE;
		ctlr->timerfunc.allocated = FALSE;
	}
	semGive(ctlr->sem);

	return OK;
}

LOCAL STATUS
i8254countget(void *cookie, UINT32 *count)
{
	I8254_CTLR *ctlr;
	UINT32 data;
	UINT8 reg;

	if (cookie == NULL || count == NULL)
		return ERROR;

	ctlr = cookie;
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	reg = Load0 | ((ctlr->selectmode << 1) & 0xff);
	data = csr8r(ctlr, T0cntr);
	data |= csr8r(ctlr, T0cntr) << 8;
	*count = data;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

LOCAL STATUS
i8254rolloverget(void *cookie, UINT32 *count)
{
	I8254_CTLR *ctlr;

	if (cookie == NULL || count == NULL)
		return ERROR;

	ctlr = cookie;
	semTake(ctlr->sem, WAIT_FOREVER);
	*count = ctlr->maxcount;
	semGive(ctlr->sem);

	return OK;
}

LOCAL STATUS
i8254isrset(void *cookie, void (*func)(_Vx_usr_arg_t), _Vx_usr_arg_t arg)
{
	I8254_CTLR *ctlr;

	if (cookie == NULL)
		return ERROR;

	ctlr = cookie;
	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	ctlr->isrfunc = func;
	ctlr->israrg = arg;
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

	return OK;
}

LOCAL STATUS
i8254enable(void *cookie, UINT32 maxtimercount)
{
	I8254_CTLR *ctlr;

	if (cookie == NULL)
		return ERROR;

	ctlr = cookie;
	semTake(ctlr->sem, WAIT_FOREVER);
	if (ctlr->enabled)
		goto out;

	if (ctlr->autoreload)
		ctlr->selectmode = ctlr->configmode;
	else
		ctlr->selectmode = 0;
	ctlr->enabled = TRUE;

	SPIN_LOCK_ISR_TAKE(&ctlr->lock);
	ctlr->maxcount = maxtimercount;
	reloadcount(ctlr, ctlr->maxcount);
	SPIN_LOCK_ISR_GIVE(&ctlr->lock);

out:
	semGive(ctlr->sem);
	return OK;
}

STATUS
i8254create(I8254_CTLR *ctlr, int unit, int mode, UINT32 minfreq, UINT32 maxfreq, UINT32 clkrate, UINT32 clkfreq)
{
	struct vxbTimerFunctionality *tf;

	SPIN_LOCK_ISR_INIT(&ctlr->lock, 0);
	ctlr->sem = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
	if (ctlr->sem == SEM_ID_NULL)
		return ERROR;

	ctlr->configmode = mode;
	ctlr->autoreload = FALSE;
	ctlr->enabled = FALSE;
	ctlr->maxcount = 0xffff;

	tf = &ctlr->timerfunc;
	snprintf(tf->timerName, sizeof(tf->timerName), "i8254-timer");
	tf->minFrequency = minfreq;
	tf->maxFrequency = maxfreq;
	tf->clkFrequency = clkfreq;
	tf->ticksPerSecond = clkrate;
	tf->rolloverPeriod = ctlr->maxcount / tf->clkFrequency;
	tf->features = VXB_TIMER_SIZE_16 |
	               VXB_TIMER_INTERMEDIATE_COUNT |
	               VXB_TIMER_AUTO_RELOAD |
	               VXB_TIMER_CAN_INTERRUPT;
	tf->timerNo = unit;
	tf->timerAllocate = i8254allocate;
	tf->timerRelease = i8254release;
	tf->timerRolloverGet = i8254rolloverget;
	tf->timerCountGet = i8254countget;
	tf->timerDisable = i8254disable;
	tf->timerEnable = i8254enable;
	tf->timerISRSet = i8254isrset;
	vxbTimerRegister(tf);

	return OK;
}

LOCAL VXB_DRV_METHOD i8254dev[] = {
	VXB_DEVMETHOD_END,
};

VXB_DRV i8254drv = {
	{ NULL },
	"i8254",
	"I8254 Timer Driver",
	VXB_BUSID_FDT,
	0,
	0,
	i8254dev,
	NULL,
};

VXB_DRV_DEF(i8254drv)

void
i8254link(void)
{
}
