/*

Dummy Mailbox driver to test the Mailbox API

DTS:

dummy_mailbox: dummy_mailbox@0 {
    compatible = "dummy_mailbox";

    #mbox-cells = <1>;

    // <phandle of the mailbox node, index of mailbox channel>
    mboxes = <&dummy_mailbox 0>,
             <&dummy_mailbox 1>;
    mbox-names = "rx", "tx";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/mailbox/vxbMailboxLib.h>

typedef struct {
	VXB_MAILBOX_DEV mailboxdev;
	VXB_DEV_ID dev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dmailboxfdt[] = {
	{ "dummy_mailbox", NULL },
	{ NULL },
};

LOCAL STATUS
chanreceive(VXB_MAILBOX_CHAN *chan, VXB_MAILBOX_MSG *msg)
{
	printf("%s(chan=%p, msg=%p)\n", __func__, chan, msg);
	return OK;
}

LOCAL STATUS
mailboxchanalloc(VXB_DEV_ID dev, VXB_MBOX_NODE *node, VXB_MAILBOX_CHAN *chan)
{
	printf("%s(dev=%p, node=%p, chan=%p)\n", __func__, dev, node, chan);
	return OK;
}

LOCAL STATUS
mailboxchanfree(VXB_DEV_ID dev, VXB_MAILBOX_CHAN *chan)
{
	printf("%s(dev=%p, chan=%p)\n", __func__, dev, chan);
	return OK;
}

LOCAL STATUS
mailboxchanrxstart(VXB_DEV_ID dev, VXB_MAILBOX_CHAN *chan)
{
	printf("%s(dev=%p, chan=%p)\n", __func__, dev, chan);
	return OK;
}

LOCAL STATUS
mailboxchansend(VXB_DEV_ID dev, VXB_MAILBOX_CHAN *chan, VXB_MAILBOX_MSG *msg)
{
	printf("%s(dev=%p, chan=%p, msg=%p)\n", __func__, dev, chan, msg);
	return OK;
}

LOCAL STATUS
mailboxchanstatusget(VXB_DEV_ID dev, VXB_MAILBOX_CHAN *chan, void *status)
{
	printf("%s(dev=%p, chan=%p, msg=%p)\n", __func__, dev, chan, status);
	return OK;
}

LOCAL STATUS
mailboxchanshutdown(VXB_DEV_ID dev, VXB_MAILBOX_CHAN *chan)
{
	printf("%s(dev=%p, chan=%p)\n", __func__, dev, chan);
	return OK;
}

LOCAL void
dmailboxfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dmailboxprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dmailboxfdt, NULL);
}

LOCAL STATUS
dmailboxattach(VXB_DEV_ID dev)
{
	VXB_MAILBOX_DEV *mailboxdev;
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	mailboxdev = &ctlr->mailboxdev;
	mailboxdev->pDev = dev;
	mailboxdev->mailboxChanAlloc = (FUNCPTR)mailboxchanalloc;
	mailboxdev->mailboxChanFree = (FUNCPTR)mailboxchanfree;
	mailboxdev->mailboxChanRxStart = (FUNCPTR)mailboxchanrxstart;
	mailboxdev->mailboxChanSend = (FUNCPTR)mailboxchansend;
	mailboxdev->mailboxChanStatusGet = (FUNCPTR)mailboxchanstatusget;
	mailboxdev->mailboxChanShutdown = (FUNCPTR)mailboxchanshutdown;

	vxbDevSoftcSet(dev, ctlr);

	if (vxbMailboxRegister(mailboxdev) != OK)
		goto error;

	return OK;

error:
	dmailboxfree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD dmailboxdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dmailboxprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dmailboxattach },
	VXB_DEVMETHOD_END,
};

VXB_DRV dmailboxdrv = {
	{ NULL },
	"dummy_mailbox",
	"Dummy Mailbox driver",
	VXB_BUSID_FDT,
	0,
	0,
	dmailboxdev,
	NULL,
};

VXB_DRV_DEF(dmailboxdrv)

STATUS
dmailboxdrvadd(void)
{
	return vxbDrvAdd(&dmailboxdrv);
}

void
dmailboxdrvtest(void)
{
	VXB_DEV_ID dev;
	VXB_MAILBOX_CHAN *rxchan, *txchan;
	VXB_MAILBOX_MSG msg;
	int status;

	dev = vxbDevAcquireByName("dummy_mailbox", 0);
	if (!dev)
		return;

	rxchan = vxbMailboxChanAlloc(dev, 0, VXB_MBOX_CHAN_DIR_RX);
	txchan = vxbMailboxChanAlloc(dev, 1, VXB_MBOX_CHAN_DIR_TX);
	printf("RX %p TX %p\n", rxchan, txchan);

	status = 0;
	vxbMailboxChanStatusGet(rxchan, &status);
	vxbMailboxChanStatusGet(txchan, &status);

	memset(&msg, 0, sizeof(msg));
	msg.msgBuf = "hello";
	msg.msgSize = 5;
	vxbMailboxChanSend(txchan, &msg);

	vxbMailboxChanSetRxHandle(rxchan, chanreceive);
	vxbMailboxChanReceive(rxchan, &msg);

	vxbMailboxChanFree(rxchan);
	vxbMailboxChanFree(txchan);
	vxbDevRelease(dev);
}
