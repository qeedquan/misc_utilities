/*

Dummy USB Target controller driver

DTS:

dummy_usb_tcd: dummy_usb_tcd@0 {
    compatible = "dummy-usb-tcd";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <usb.h>
#include <usb.h>
#include <ossLib.h>
#include <usbOsal.h>
#include <usbOsalDebug.h>
#include <usbPlatform.h>
#include <usbTgt.h>

typedef struct {
	VXB_DEV_ID dev;
} Ctlr;

LOCAL STATUS
create_pipe(pUSBTGT_TCD pTCD, pUSB_ENDPOINT_DESCR pEndpointDesc, UINT16 uConfigurationValue, UINT16 uInterface, UINT16 uAltSetting, pUSB_TARG_PIPE pPipeHandle)
{
	return OK;
}

LOCAL STATUS
delete_pipe(pUSBTGT_TCD pTCD, USB_TARG_PIPE pipeHandle)
{
	return OK;
}

LOCAL STATUS
submit_erp(pUSBTGT_TCD pTCD, USB_TARG_PIPE pipeHandle, pUSB_ERP pErp)
{
	return OK;
}

LOCAL STATUS
cancel_erp(pUSBTGT_TCD pTCD, USB_TARG_PIPE pipeHandle, pUSB_ERP pErp)
{
	return OK;
}

LOCAL STATUS
pipe_status_set(pUSBTGT_TCD pTCD, USB_TARG_PIPE pipeHandle, UINT16 uStatus)
{
	return OK;
}

LOCAL STATUS
pipe_status_get(pUSBTGT_TCD pTCD, USB_TARG_PIPE pipeHandle, UINT16 *pStatus)
{
	return OK;
}

LOCAL STATUS
get_frame_num(pUSBTGT_TCD pTCD, UINT16 *pFrameNum)
{
	return OK;
}

LOCAL STATUS
xioctl(pUSBTGT_TCD pTcd, int cmd, void *pContext)
{
	return OK;
}

LOCAL STATUS
wakeup(pUSBTGT_TCD pTCD)
{
	return OK;
}

LOCAL STATUS
soft_connect(pUSBTGT_TCD pTCD, BOOL bConnectUp)
{
	return OK;
}

LOCAL const VXB_FDT_DEV_MATCH_ENTRY tcdfdt[] = {
	{ "dummy-usb-tcd", NULL },
	{ NULL },
};

LOCAL void
tcdfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
tcdprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, tcdfdt, NULL);
}

LOCAL STATUS
tcdattach(VXB_DEV_ID dev)
{
	static USBTGT_TCD_FUNCS tcdfuncs = {
		create_pipe,
		delete_pipe,
		submit_erp,
		cancel_erp,
		pipe_status_set,
		pipe_status_get,
		wakeup,
		get_frame_num,
		soft_connect,
		xioctl,
	};

	Ctlr *ctlr;
	USBTGT_TCD_INFO info;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	memset(&info, 0, sizeof(info));
	info.pDev = dev;
	info.pTcdFuncs = &tcdfuncs;
	info.pTcdSpecific = dev;

	info.uVendorID = 0x1234;
	info.uProductID = 0x5678;
	info.uBcdDevice = 0;
	info.pMfgString = "x";
	info.pProdString = "y";
	info.pSerialString = "z";
	info.pTcdName = "a";
	info.uTcdUnit = 0;
	info.uConfigCount = 0;

	usbTgtTcdRegister(&info);

	return OK;

error:
	tcdfree(ctlr);
	return ERROR;
}

LOCAL STATUS
tcddetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	tcdfree(ctlr);
	return OK;
}

LOCAL STATUS
tcdshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD tcddev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), tcdprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), tcdattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), tcddetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), tcdshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV tcddrv = {
	{ NULL },
	"dummy-usb-tcd",
	"Dummy USB TCD driver",
	VXB_BUSID_FDT,
	0,
	0,
	tcddev,
	NULL,
};

VXB_DRV_DEF(tcddrv)

STATUS
tcddrvadd(void)
{
	STATUS status;

	status = vxbDrvAdd(&tcddrv);
	return status;
}
