#include "dat.h"
#include "fns.h"

#if defined(INCLUDE_DEBUG_KPRINTF) || defined(INCLUDE_DEBUG_KPUTS)
#define PGS2_DESC_FLAG_DEF BOARD_DESC_FLAG(BOARD_DESC_FLAG_DBG, 0x00)
#else
#define PGS2_DESC_FLAG_DEF 0
#endif

#define DEBUG_EARLY_PRINT

LOCAL void pgs2earlyinit(void);
LOCAL void pgs2init(void);
LOCAL void pgs2infoshow(void);

BOARD_FUNC_TBL pgs2funcs = {
	.earlyInit = pgs2earlyinit,
	.init = pgs2init,
	.reset = mv64360Reset,
	.model = mv64360Model,
	.usDelay = __vxCpuUsDelay,
	.nsDelay = __vxCpuNsDelay,
	.nsRes = __vxCpuNsRes,
#ifdef INCLUDE_SHOW_ROUTINES
	.infoShow = pgs2infoshow,
#endif
	.endMacGet = NULL,
};

LOCAL BOARD_DESC pgs2board = {
	.uVer = BOARD_DESC_VER_4_0,
	.pCompat = "pegasos2",
	.uFlag = PGS2_DESC_FLAG_DEF,
	.probe = mv64360Probe,
	.pFuncTbl = &pgs2funcs,
};

BOARD_DEF(pgs2board)

LOCAL void *mvbase;
LOCAL UINT8 *iobase;

UINT32
mvread32(UINT32 addr)
{
	return vxbRead32((void *)VXB_HANDLE_ORDERED, (UINT32 *)((char *)mvbase + addr));
}

void
mvwrite32(UINT32 addr, UINT32 value)
{
	vxbWrite32((void *)VXB_HANDLE_ORDERED, (UINT32 *)((char *)mvbase + addr), value);
}

LOCAL STATUS
debugwrite(char *buf, size_t len)
{
	UINT8 *uartbase;
	void *handle;
	size_t i;

	uartbase = iobase + 0x2f8;
	handle = (void *)VXB_HANDLE_ORDERED;
	for (i = 0; i < len; i++) {
		while (!(vxbRead8(handle, uartbase + LSR) & LSR_THRE))
			;
		vxbWrite8(handle, uartbase + THR, buf[i]);
	}

	return OK;
}

LOCAL void
pgs2earlyinit(void)
{
	mv64360EarlyInit();
}

LOCAL void
pgs2init(void)
{
	mv64360Init();

	mvbase = pmapGlobalMap(SYS_REGS_BASE, SYS_REGS_SIZE, MMU_ATTR_SUP_RW | MMU_ATTR_CACHE_OFF | MMU_ATTR_CACHE_GUARDED);

	// all GPP pins are input
	mvwrite32(MV64360_GPP_IO_CTRL, 0);

	// all GPP levels are active high
	mvwrite32(MV64360_GPP_LVL_CTRL, 0);

	// allow for all GPP interrupts (this won't cause any interrupts until the system is ready to handle interrupts)
	// the southbridge interrupts are routed through the GPP
	mvwrite32(MV64360_GPP_INT_MASK0, 0xffffffff);

#ifdef DEBUG_EARLY_PRINT
	iobase = pmapGlobalMap(0xfe000000, 0x1000, MMU_ATTR_SUP_RW | MMU_ATTR_CACHE_OFF | MMU_ATTR_CACHE_GUARDED);
	_func_kwrite = debugwrite;
#endif

	utilinit();
}

LOCAL char *
getcoremodel(UINT32 pvr)
{
	switch (vxPvrGet() & _PPC_PVR_VER_MASK) {
	case _PPC_PVR_PPC750GX:
		return "PPC750GX";
	}
	return NULL;
}

LOCAL void
pgs2infoshow(void)
{
	UINT32 pvr;
	char *coremodel;

	pvr = vxPvrGet();
	coremodel = getcoremodel(pvr);
	if (coremodel == NULL)
		coremodel = "UNKNOWN";

	printf("CORE: %s (PVR: %x)\n", coremodel, pvr);
}

void
pgs2links(void)
{
	pgs2pcilink();
	vt8231link();
	viaidelink();
	i8254link();
	ds1385link();
	i6300wdlink();
	viai2clink();
	nvramlink();
	lptlink();
	m48t59link();
	utillink();
}
