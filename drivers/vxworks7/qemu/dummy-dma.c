/*

Dummy DMA driver to test the DMA API

DTS:

dummy_dma: dma-controller@0 {
    compatible = "dummy_dma";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/dma/vxbDmaLib.h>

typedef struct {
	VXB_DMA_CHAN dmachan;
} Chan;

typedef struct {
	VXB_DEV_ID dev;
	VXB_DMA_DEV dmadev;
	Chan chans[16];
	size_t nchan;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY ddmafdt[] = {
	{ "dummy_dma", NULL },
	{ NULL },
};

LOCAL BOOL
matcher(VXB_DEV_ID dev, void *param)
{
	printf("%s(dev=%p, param=%p)\n", __func__, dev, param);
	return FALSE;
}

LOCAL VXB_DMA_CHAN *
dmachanalloc(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	void *chan;

	printf("%s(dev=%p)\n", __func__, dev);
	ctlr = vxbDevSoftcGet(dev);
	chan = NULL;
	if (ctlr->nchan < NELEMENTS(ctlr->chans))
		chan = &ctlr->chans[ctlr->nchan++];

	return chan;
}

LOCAL VXB_DMA_CHAN *
dmachanallocadvance(VXB_DEV_ID dev, int chan, UINT32 cap)
{
	printf("%s(dev=%p, chan=%d, cap=%" PRId32 ")\n", __func__, dev, chan, cap);
	return NULL;
}

LOCAL STATUS
dmachanfree(VXB_DEV_ID dev, VXB_DMA_CHAN *dmachan)
{
	printf("%s(dev=%p, dmachan=%p)\n", __func__, dev, dmachan);
	return OK;
}

LOCAL VXB_DMA_TX *
dmachanprememcpy(VXB_DEV_ID dev, VXB_DMA_CHAN *dmachan, VIRT_ADDR src, VIRT_ADDR dest, size_t size, int flag)
{
	printf("%s(dev=%p, dmachan=%p, src=%#lx, dest=%#lx, size=%zu, flag=%#x)\n",
	    __func__, dev, dmachan, (unsigned long)src, (unsigned long)dest, size, flag);
	return NULL;
}

LOCAL STATUS
dmachancontrol(VXB_DEV_ID dev, VXB_DMA_CHAN *dmachan, VXB_DMA_CTRL_CMD cmd, void *arg)
{
	printf("%s(dev=%p, dmachan=%p, cmd=%d, arg=%p)\n",
	    __func__, dev, dmachan, cmd, arg);
	return OK;
}

LOCAL STATUS
dmactrlcapget(VXB_DEV_ID dev, VXB_DMA_CTRL_CAP *cap)
{
	Ctlr *ctlr;

	printf("%s(dev=%p, cap=%p)\n", __func__, dev, cap);

	ctlr = vxbDevSoftcGet(dev);
	memset(cap, 0, sizeof(*cap));
	cap->maxChansPerDev = NELEMENTS(ctlr->chans);
	cap->maxBurstLen = 9000;
	cap->maxBurstSize = 9001;

	return OK;
}

LOCAL void
ddmafree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
ddmaprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, ddmafdt, NULL);
}

LOCAL STATUS
ddmaattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	size_t i;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->dmadev.pDev = dev;
	ctlr->dmadev.flag |= DMA_DEV_HW_COHERENT;
	for (i = 0; i < NELEMENTS(ctlr->chans); i++)
		vxbDmaChanAdd(&ctlr->dmadev, (VXB_DMA_CHAN *)&ctlr->chans[i]);

	vxbDevSoftcSet(dev, ctlr);

	vxbDmaRegister(&ctlr->dmadev);

	return OK;

error:
	ddmafree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD ddmadev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), ddmaprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), ddmaattach },

	{ VXB_DEVMETHOD_CALL(vxbDmaChanAlloc), (FUNCPTR)dmachanalloc },
	{ VXB_DEVMETHOD_CALL(vxbDmaChanAllocAdvance), (FUNCPTR)dmachanallocadvance },
	{ VXB_DEVMETHOD_CALL(vxbDmaChanFree), (FUNCPTR)dmachanfree },
	{ VXB_DEVMETHOD_CALL(vxbDmaChanPreMemCpy), (FUNCPTR)dmachanprememcpy },
	{ VXB_DEVMETHOD_CALL(vxbDmaChanControl), (FUNCPTR)dmachancontrol },
	{ VXB_DEVMETHOD_CALL(vxbDmaCtrlCapGet), (FUNCPTR)dmactrlcapget },

	VXB_DEVMETHOD_END,
};

VXB_DRV ddmadrv = {
	{ NULL },
	"dummy_dma",
	"Dummy DMA driver",
	VXB_BUSID_FDT,
	0,
	0,
	ddmadev,
	NULL,
};

VXB_DRV_DEF(ddmadrv)

STATUS
ddmadrvadd(void)
{
	return vxbDrvAdd(&ddmadrv);
}

void
ddmadrvtest(void)
{
	VXB_DMA_CTRL_CAP cap;

	vxbDmaDevGetByMatch(matcher, NULL);
	if (vxbDmaCtrlCapGet("dummy_dma", 0, &cap) == OK) {
		printf("Capabilities:\n");
		printf("\tMax channels per device: %" PRId32 "\n", cap.maxChansPerDev);
		printf("\tMax burst length: %" PRId32 "\n", cap.maxBurstLen);
		printf("\tMax burst size: %" PRId32 "\n", cap.maxBurstSize);
		printf("\tPause resume support: %" PRId32 "\n", cap.pauseResumeSupport);
	}

	vxbDmaChanAlloc(0);
	vxbDmaChanAllocAdvance("dummy_dma", 0, 10, 0);
}
