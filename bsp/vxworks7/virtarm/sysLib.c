#include "dat.h"
#include "fns.h"

#if defined(INCLUDE_DEBUG_KPRINTF) || defined(INCLUDE_DEBUG_KPUTS)
#define VIRTARM_DESC_FLAG_DEF BOARD_DESC_FLAG(BOARD_DESC_FLAG_DBG, 0)
#else
#define VIRTARM_DESC_FLAG_DEF BOARD_DESC_FLAG(0, 0)
#endif

#define DEBUG_EARLY_PRINT

LOCAL BOOL virtarmprobe(char *);
LOCAL void virtarmearlyinit(void);
LOCAL void virtarminit(void);
LOCAL void virtarmreset(int);
LOCAL char *virtarmmodel(void);
LOCAL void virtarmusdelay(int);
LOCAL void virtarminfoshow(void);

LOCAL BOARD_FUNC_TBL virtarmfuncs = {
	.earlyInit = virtarmearlyinit,
	.init = virtarminit,
	.reset = virtarmreset,
	.model = virtarmmodel,
	.usDelay = virtarmusdelay,
	.nsDelay = __vxCpuNsDelay,
	.nsRes = __vxCpuNsRes,

#ifdef INCLUDE_SHOW_ROUTINES
	.infoShow = virtarminfoshow,
#endif

	.endMacGet = NULL
};

LOCAL BOARD_DESC virtarm = {
	.uVer = BOARD_DESC_VER_4_0,
	.pCompat = "linux,dummy-virt",
	.uFlag = VIRTARM_DESC_FLAG_DEF,
	.probe = virtarmprobe,
	.pFuncTbl = &virtarmfuncs
};

BOARD_DEF(virtarm)

LOCAL UINT32 *uartbase;

LOCAL STATUS
debugwrite(char *buf, size_t len)
{
	enum {
		DR = 0x0 / 4,
		FR = 0x18 / 4,

		FR_TXFF = 0x20,
		FR_RXFE = 0x10,
		FR_BUSY = 0x08,
	};

	void *handle;
	size_t i;

	if (buf == NULL || uartbase == NULL)
		return ERROR;

	handle = (void *)VXB_HANDLE_ORDERED;
	for (i = 0; i < len; i++) {
		while ((vxbRead32(handle, uartbase + FR) & FR_TXFF))
			;
		vxbWrite32(handle, uartbase + DR, buf[i]);
	}

	return OK;
}

LOCAL BOOL
virtarmprobe(char *compat)
{
	int offset;

	if ((offset = vxFdtPathOffset("/")) < 0)
		return FALSE;
	return vxFdtNodeCheckCompatible(offset, compat) == 0;
}

LOCAL void
virtarmearlyinit(void)
{
	UINT state;

	state = MMU_ATTR_VALID | MMU_ATTR_SUP_RWX | MMU_ATTR_CACHE_COPYBACK | MMU_ATTR_CACHE_COHERENCY;
	sysPhysMemDescNumEnt = vxFdtPhysMemInfoGet(sysPhysMemDesc, sysPhysMemDescNumEnt, state);
}

LOCAL STATUS
earlyconsole(void)
{
	PHYS_ADDR physaddr;
	size_t physlen;
	int offset;

	offset = vxFdtStdoutGet();
	if (offset < 0)
		return ERROR;

	if (vxFdtNodeCheckCompatible(offset, "arm,pl011") != 0)
		return ERROR;

	if (vxFdtDefRegGet(offset, 0, &physaddr, &physlen) != OK)
		return ERROR;

	uartbase = pmapGlobalMap(physaddr, physlen, VXB_REG_MAP_MMU_ATTR);
	if (uartbase == PMAP_FAILED) {
		uartbase = NULL;
		return ERROR;
	}
	_func_kwrite = debugwrite;

	return OK;
}

LOCAL void
virtarminit(void)
{
#ifdef DEBUG_EARLY_PRINT
	earlyconsole();
#endif
}

LOCAL void
virtarmusdelay(int us)
{
	__vxCpuNsDelay(us * 1000);
}

LOCAL void
virtarmreset(int starttype)
{
}

LOCAL char *
virtarmmodel(void)
{
	int offset;
	char *value;

	offset = vxFdtPathOffset("/");
	if (offset < 0)
		return "unknown board";

	value = (char *)vxFdtPropGet(offset, "model", NULL);
	return (value) ? value : "unknown board";
}

LOCAL void
virtarminfoshow(void)
{
	printf("Board name: QEMU Virt ARM\n");
	cpuArmVerShow();
}

void
virtarmlinks(void)
{
	pl031link();
	pl061link();
}