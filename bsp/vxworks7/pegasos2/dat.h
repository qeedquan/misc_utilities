#ifndef _DAT_H_
#define _DAT_H_

#include <vxWorks.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <time.h>
#include <inttypes.h>
#include <endian.h>
#include <boardLib.h>
#include <spinLockLib.h>
#include <intLib.h>
#include <sysLib.h>
#include <semLib.h>
#include <pmapLib.h>
#include <mv64360.h>
#include <ioLib.h>
#include <iosLib.h>
#include <fsMonitor.h>
#include <xbdPartition.h>
#include <fsEventUtilLib.h>
#include <vxbSataLib.h>
#include <hwif/buslib/pciDefines.h>
#include <subsys/int/vxbIntLib.h>
#include <subsys/timer/vxbTimerLib.h>
#include <subsys/clk/vxbClkLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbFdtPci.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/buslib/vxbFdtI2cLib.h>
#include <hwif/drv/sio/vxbNs16550Sio.h>
#include <hwif/drv/resource/vxbRtcLib.h>
#include <hwif/drv/watchdog/vxbWatchdog.h>
#include <hwif/drv/watchdog/watchdog.h>
#include <arch/ppc/vxPpcLib.h>
#include <arch/ppc/mmuPpcLib.h>
#include <private/timeP.h>
#include <end.h>
#include <muxLib.h>
#include <tyLib.h>

enum {
	LPT_SETCONTROL = 0,
	LPT_GETSTATUS,
};

enum {
	IRQS = 16,
};

typedef struct {
	// must be first field
	struct vxbTimerFunctionality timerfunc;

	VXB_DEV_ID dev;
	VXB_RESOURCE *intres;

	BOOL enabled;
	BOOL autoreload;

	void *reghandle;
	VIRT_ADDR regbase;

	// timer counting mode
	int selectmode;
	int configmode;

	// max counter value for the timer
	UINT32 maxcount;

	// isr custom function
	void (*isrfunc)(_Vx_usr_arg_t);
	_Vx_usr_arg_t israrg;

	SEM_ID sem;
	spinlockIsr_t lock;
} I8254_CTLR;

typedef struct {
	// must be first field
	struct vxbTimerFunctionality timerfunc;

	VXB_I2C_RTC_FUNC rtcfunc;

	VXB_DEV_ID dev;
	VXB_RESOURCE *intres;

	BOOL timerenabled;

	void *reghandle;
	VIRT_ADDR regbase;

	void (*isrfunc)(_Vx_usr_arg_t);
	_Vx_usr_arg_t israrg;

	RTC_ALARM_FUNC alarmfunc;
	void *alarmarg;

	UINT8 savedint;

	spinlockIsr_t lock;
} DS1385_CTLR;

typedef struct {
	// need to be the first field
	NS16550VXB_CHAN chan;

	VXB_DEV_ID dev;
	VXB_RESOURCE *intres;
} SIO_CTLR;

typedef struct {
	// need to be the first field
	DEV_HDR devhdr;

	VXB_DEV_ID dev;
	VXB_RESOURCE *intres;

	void *reghandle;
	VIRT_ADDR regbase;

	int iosdrvnum;

	BOOL autofeed;

	SEM_ID semsync;
	SEM_ID semio;
} LPT_CTLR;

typedef struct {
	VXB_DEV_ID dev;

	void *reghandle;
	VIRT_ADDR regbase;

	VXB_I2C_RTC_FUNC rtcfunc;

	int nvramsize;

	char wdname[MAX_DRV_NAME_LEN];
	int wdunit;
	int wdtimeout;

	spinlockIsr_t lock;
} M48T59_CTLR;

typedef struct {
	VXB_DEV_ID dev;
	int fdtoff;

	VXB_RESOURCE *res;
	void *reghandle;
	VIRT_ADDR regbase;

	VXB_RESOURCE *intres;

	VXB_RESOURCE reslist[8];
	VXB_RESOURCE_ADR resadrlist[8];
	VXB_RESOURCE_IRQ resirqlist[8];
	VXB_INTR_ENTRY intrentry[8];
	size_t nresalloc;

	SIO_CTLR sio;
	I8254_CTLR timer;
	DS1385_CTLR rtc;
	LPT_CTLR lpt;
	M48T59_CTLR m48t59;

	UINT32 intbase;
	UINT32 intnmi;
	atomic_ullong intstats[IRQS];
} VT8231_CTLR;

#endif
