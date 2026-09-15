#include "dat.h"
#include "fns.h"

enum {
	// 8259 interrupt controllers
	PIC1 = 0x20,
	PIC2 = 0xA0,

	CTL = 0x0,
	DATA = 0x1,

	EOI = 0x20,
};

IMPORT STATUS ns16550Create(NS16550VXB_CHAN *chan);
IMPORT STATUS ns16550vxbInt(VXB_DEV_ID dev);

#define csr8r(c, a) vxbRead8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))))
#define csr8w(c, a, v) vxbWrite8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))), v)

LOCAL const VXB_PCI_DEV_MATCH_ENTRY vt8231pci[] = {
	{ 0x8231, 0x1106, NULL },
	{},
};

LOCAL int
fdtregbase(VT8231_CTLR *ctlr, const char *name)
{
	const UINT32 *regprop;
	int reglen;
	int off;

	off = vxFdtGetSubnodeByName(ctlr->fdtoff, name);
	if (off < 0)
		return -1;

	regprop = vxFdtPropGet(off, "reg", &reglen);
	if (regprop == NULL || reglen != 12)
		return -1;

	if (!(vxFdt32ToCpu(regprop[0]) & 0x1))
		return -1;

	return vxFdt32ToCpu(regprop[1]);
}

LOCAL void
vt8231int(void *ctx)
{
	VT8231_CTLR *ctlr;
	UINT32 irq;
	UINT32 i;

	ctlr = ctx;
	irq = csr8r(ctlr, PIC1 + CTL);
	if (irq & 0x4)
		irq |= csr8r(ctlr, PIC2 + CTL) << 8;
	irq = (irq | ctlr->intnmi) & ~0x4;

	for (i = 0; i < NELEMENTS(ctlr->intstats); i++) {
		if (!(irq & (1 << i)))
			continue;

		ctlr->intstats[i]++;
		VXB_INT_ISR_CALL(ctlr->intbase + i);
	}

	mvread32(MV64360_PCI1_INT_ACK_VIRT);
	csr8w(ctlr, PIC1 + CTL, EOI);
	csr8w(ctlr, PIC2 + CTL, EOI);
}

LOCAL void
vt8231free(VT8231_CTLR *ctlr)
{
	if (!ctlr)
		return;

	vxbResourceFree(ctlr->dev, ctlr->res);
	vxbResourceFree(ctlr->dev, ctlr->intres);
	vxbDevDestroy(ctlr->sio.dev);
	vxbMemFree(ctlr);
}

LOCAL STATUS
vt8231probe(VXB_DEV_ID dev)
{
	return vxbPciDevMatch(dev, vt8231pci, NULL);
}

LOCAL STATUS
addresources(VXB_DEV_ID dev, VT8231_CTLR *ctlr)
{
	VXB_FDT_DEV *fdtdev;
	PCI_HARDWARE *pcidev;
	VXB_DEV_ID parent;
	VXB_RESOURCE *res;
	VXB_RESOURCE_IRQ *resirq;
	VXB_INTR_ENTRY *intrentry;

	parent = vxbDevParent(dev);
	fdtdev = vxbFdtDevGet(parent);
	pcidev = vxbDevIvarsGet(dev);

	if (pgs2pciaddio(dev, &ctlr->reslist[0], &ctlr->resadrlist[0]) != OK)
		return ERROR;

	// interrupts are routed through GPP
	res = &ctlr->reslist[1];
	resirq = &ctlr->resirqlist[1];
	intrentry = &ctlr->intrentry[1];
	res->pRes = resirq;
	res->id = VXB_RES_ID_CREATE(VXB_RES_IRQ, 0);
	resirq->pVxbIntrEntry = intrentry;
	resirq->hVec = 0x31;
	resirq->lVec = resirq->hVec;
	intrentry->node = vxbFdtIntParentOffsetGetByDev(fdtdev);
	if (vxbResourceAdd(&pcidev->vxbResList, res) != OK)
		return ERROR;

	ctlr->nresalloc = 2;

	return OK;
}

LOCAL STATUS
makedev(VXB_DEV_ID dev, VT8231_CTLR *ctlr, const char *fdtname, char *childname, VXB_DEV_ID *child, void *childctlr, void **reghandle, VIRT_ADDR *regbase)
{
	int base;

	base = fdtregbase(ctlr, fdtname);
	if (base < 0)
		return ERROR;

	if (vxbDevCreate(child) != OK)
		return ERROR;
	if (vxbDevAdd(dev, *child) != OK)
		return ERROR;

	vxbDevNameSet(*child, childname, FALSE);
	vxbDevSoftcSet(*child, childctlr);

	*reghandle = ctlr->reghandle;
	*regbase = ctlr->regbase + base;

	return OK;
}

LOCAL STATUS
addirq(VT8231_CTLR *ctlr, VXB_DEV_ID dev, UINT32 hvec, VXB_RESOURCE **intres, VOIDFUNCPTR func)
{
	VXB_RESOURCE *res;
	VXB_RESOURCE_IRQ *resirq;
	VXB_INTR_ENTRY *intrentry;

	res = &ctlr->reslist[ctlr->nresalloc];
	resirq = &ctlr->resirqlist[ctlr->nresalloc];
	intrentry = &ctlr->intrentry[ctlr->nresalloc];
	res->pRes = resirq;
	res->id = VXB_RES_ID_CREATE(VXB_RES_IRQ, 0);
	resirq->pVxbIntrEntry = intrentry;
	resirq->hVec = hvec;
	resirq->lVec = ctlr->intbase + resirq->hVec;
	ctlr->nresalloc++;

	*intres = res;
	if (vxbIntConnect(dev, *intres, func, dev) != OK)
		return ERROR;

	if (vxbIntEnable(dev, *intres) != OK)
		return ERROR;

	return OK;
}

LOCAL STATUS
addsio(VXB_DEV_ID dev, VT8231_CTLR *ctlr)
{
	SIO_CTLR *sio;
	NS16550VXB_CHAN *chan;
	void *reghandle;
	VIRT_ADDR regbase;

	sio = &ctlr->sio;
	if (makedev(dev, ctlr, "serial", "16550-sio", &sio->dev, sio, &reghandle, &regbase) != OK)
		return ERROR;

	chan = &sio->chan;
	chan->baudRate = 115200;
	chan->xtal = chan->baudRate * 16;
	chan->regDelta = 1;
	chan->fifoSize = NS16550_FIFO_DEPTH_DEFAULT;
	chan->flag = 0;
	chan->pDev = sio->dev;
	chan->bar = regbase;
	chan->adrRes = NULL;
	chan->intRes = NULL;
	chan->handle = reghandle;
	ns16550Create(chan);

	if (addirq(ctlr, sio->dev, 4, &sio->intres, (VOIDFUNCPTR)ns16550vxbInt) != OK)
		return ERROR;

	return OK;
}

LOCAL STATUS
addtimer(VXB_DEV_ID dev, VT8231_CTLR *ctlr)
{
	I8254_CTLR *timer;

	timer = &ctlr->timer;
	if (makedev(dev, ctlr, "timer", "i8254-timer", &timer->dev, timer, &timer->reghandle, &timer->regbase) != OK)
		return ERROR;

	if (i8254create(timer, 0, 2, 19, 5000, 60, 1193182) != OK)
		return ERROR;

	if (addirq(ctlr, timer->dev, 0, &timer->intres, i8254int) != OK)
		return ERROR;

	return OK;
}

LOCAL STATUS
addrtc(VXB_DEV_ID dev, VT8231_CTLR *ctlr)
{
	DS1385_CTLR *rtc;

	rtc = &ctlr->rtc;
	if (makedev(dev, ctlr, "rtc", "ds1385-rtc", &rtc->dev, rtc, &rtc->reghandle, &rtc->regbase) != OK)
		return ERROR;

	if (addirq(ctlr, rtc->dev, 8, &rtc->intres, ds1385int) != OK)
		return ERROR;

	return ds1385create(rtc, 0, 2, 8192, 32768);
}

LOCAL STATUS
addlpt(VXB_DEV_ID dev, VT8231_CTLR *ctlr)
{
	LPT_CTLR *lpt;

	lpt = &ctlr->lpt;
	if (makedev(dev, ctlr, "lpt", "lpt", &lpt->dev, lpt, &lpt->reghandle, &lpt->regbase) != OK)
		return ERROR;

	if (addirq(ctlr, lpt->dev, 7, &lpt->intres, lptint) != OK)
		return ERROR;

	return lptcreate(lpt);
}

LOCAL STATUS
addm48t59(VXB_DEV_ID dev, VT8231_CTLR *ctlr)
{
	M48T59_CTLR *m48t59;

	m48t59 = &ctlr->m48t59;
	if (makedev(dev, ctlr, "m48t59", "m48t59", &m48t59->dev, m48t59, &m48t59->reghandle, &m48t59->regbase) != OK)
		return ERROR;

	return m48t59create(m48t59);
}

LOCAL STATUS
vt8231attach(VXB_DEV_ID dev)
{
	VT8231_CTLR *ctlr;
	VXB_RESOURCE_ADR *resadr;
	VXB_DEV_ID parent;
	VXB_FDT_DEV *fdtdev;

	parent = vxbDevParent(dev);
	fdtdev = vxbFdtDevGet(parent);
	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr || parent == NULL || fdtdev == NULL)
		goto error;

	ctlr->fdtoff = vxFdtGetSubnodeByName(fdtdev->offset, "isa");
	if (ctlr->fdtoff < 0)
		goto error;

	if (addresources(dev, ctlr) != OK)
		goto error;

	ctlr->res = vxbResourceAlloc(dev, VXB_RES_IO, 0);
	ctlr->intres = vxbResourceAlloc(dev, VXB_RES_IRQ, 0);
	if (ctlr->res == NULL || ctlr->intres == NULL)
		goto error;

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual;
	ctlr->reghandle = resadr->pHandle;
	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	if (vxbIntRegister(dev, ctlr->fdtoff, IRQS, 0, &ctlr->intbase) != OK)
		goto error;

	addsio(dev, ctlr);
	addtimer(dev, ctlr);
	addrtc(dev, ctlr);
	addlpt(dev, ctlr);
	addm48t59(dev, ctlr);

	vxbIntConnect(ctlr->dev, ctlr->intres, vt8231int, ctlr);
	vxbIntEnable(ctlr->dev, ctlr->intres);

	return OK;

error:
	vt8231free(ctlr);
	return ERROR;
}

LOCAL void
intmask(VT8231_CTLR *ctlr, UINT32 vector, BOOL enable)
{
	UINT8 pic, bit, val;

	if (vector < 8)
		pic = PIC1;
	else {
		pic = PIC2;
		vector -= 8;
	}

	bit = (1 << vector) & 0xff;
	val = csr8r(ctlr, pic + DATA) | bit;
	if (enable)
		val &= ~bit;
	csr8w(ctlr, pic + DATA, val);
}

LOCAL STATUS
vt8231intenable(VXB_DEV_ID dev, UINT32 vector, VXB_INTR_ENTRY *entry)
{
	VT8231_CTLR *ctlr;
	int key;

	if (vector >= IRQS)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	key = INT_CPU_LOCK();

	// there is a weird state that the serial port can get into where it does not generate interrupts even when there is input
	// no output can take place in that state, easiest fix is to force the serial interrupt handler to always run
	// wastes more cycles but fixes the issue
	if (vector == 4)
		ctlr->intnmi |= (1 << vector);
	intmask(ctlr, vector, TRUE);

	INT_CPU_UNLOCK(key);

	return OK;
}

LOCAL STATUS
vt8231intdisable(VXB_DEV_ID dev, UINT32 vector, VXB_INTR_ENTRY *entry)
{
	VT8231_CTLR *ctlr;
	int key;

	if (vector >= IRQS)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	key = INT_CPU_LOCK();

	ctlr->intnmi &= ~(1 << vector);
	intmask(ctlr, vector, FALSE);

	INT_CPU_UNLOCK(key);

	return OK;
}

LOCAL VXB_DRV_METHOD vt8231dev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), vt8231probe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), vt8231attach },
	{ VXB_DEVMETHOD_CALL(vxbIntEnable), vt8231intenable },
	{ VXB_DEVMETHOD_CALL(vxbIntDisable), vt8231intdisable },
	VXB_DEVMETHOD_END,
};

VXB_DRV vt8231drv = {
	{ NULL },
	"vt8231-isa",
	"VIA VT8231 ISA Driver",
	VXB_BUSID_PCI,
	0,
	0,
	vt8231dev,
	NULL,
};

VXB_DRV_DEF(vt8231drv)

void
vt8231show(void)
{
	VXB_DEV_ID dev;
	VT8231_CTLR *ctlr;
	size_t i;

	dev = vxbDevAcquireByName("vt8231-isa", 0);
	if (!dev)
		return;

	ctlr = vxbDevSoftcGet(dev);
	printf("I8259 #1 DATA: %#x\n", csr8r(ctlr, PIC1 + DATA));
	printf("I8259 #2 DATA: %#x\n\n", csr8r(ctlr, PIC2 + DATA));
	printf("Stats\n");
	for (i = 0; i < NELEMENTS(ctlr->intstats); i++)
		printf("IRQ %zu: count %llu\n", i, ctlr->intstats[i]);
	printf("\n");

	vxbDevRelease(dev);
}

void
vt8231link(void)
{
}
