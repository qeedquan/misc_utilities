/*

Dummy USB target function driver

The hierarchy for USB target functions is as follows:
USB Target Controller Hardware ->
  USB Target Controller Driver (TCD) ->
    USB Target Management Layer (TML) ->
      USB Target Function Driver (TFD)

*/

#include <vxWorks.h>
#include <semLib.h>
#include <logLib.h>
#include <errno.h>
#include <errnoLib.h>
#include <iosLib.h>
#include <rngLib.h>
#include <selectLib.h>
#include <wdLib.h>
#include <usbCdc.h>
#include <usbTgt.h>
#include <usbTgtFunc.h>
#include <drv/erf/erfLib.h>

typedef struct {
	USB_TARG_CHANNEL targChannel;
} USB_TGT_DEV;

LOCAL pUSB_DESCR_HDR FsFuncDescHdr[10];
LOCAL pUSB_DESCR_HDR HsFuncDescHdr[10];
LOCAL pUSB_DESCR_HDR SsFuncDescHdr[10];
LOCAL USB_TGT_DEV UsbDev;

LOCAL STATUS
MngmtFunc(
    pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT16 code,
    pVOID context)
{
	switch (code) {
	case TARG_MNGMT_ATTACH:
	case USBTGT_NOTIFY_ATTACH_EVENT:
		break;
	case TARG_MNGMT_DETACH:
	case USBTGT_NOTIFY_DETACH_EVENT:
		break;
	case TARG_MNGMT_BUS_RESET:
	case USBTGT_NOTIFY_RESET_EVENT:
		break;
	case TARG_MNGMT_DISCONNECT:
	case USBTGT_NOTIFY_DISCONNECT_EVENT:
		break;
	case TARG_MNGMT_SUSPEND:
	case USBTGT_NOTIFY_SUSPEND_EVENT:
		break;
	case TARG_MNGMT_RESUME:
	case USBTGT_NOTIFY_RESUME_EVENT:
		break;
	}
	return OK;
}

LOCAL STATUS
FeatureClear(
    pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT8 req,
    UINT16 feature,
    UINT16 index)
{
	return OK;
}

LOCAL STATUS
FeatureSet(pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT8 request,
    UINT16 feature,
    UINT16 index)
{
	return OK;
}

LOCAL STATUS
ConfigurationSet(
    pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT8 configuration)
{
	return OK;
}

LOCAL STATUS
DescriptorGet(
    pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT8 requestType,
    UINT8 descriptorType,
    UINT8 descriptorIndex,
    UINT16 languageId,
    UINT16 length,
    pUINT8 pBfr,
    pUINT16 pActLen)
{
	return OK;
}

LOCAL STATUS
InterfaceGet(
    pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT16 interfaceIndex,
    pUINT8 pAlternateSetting)
{
	return OK;
}

LOCAL STATUS
InterfaceSet(
    pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT16 interfaceIndex,
    UINT8 alternateSetting)
{
	return OK;
}

LOCAL STATUS
VendorSpecific(
    pVOID param,
    USB_TARG_CHANNEL targChannel,
    UINT8 requestType,
    UINT8 request,
    UINT16 value,
    UINT16 index,
    UINT16 length)
{
	return OK;
}

LOCAL USB_TARG_CALLBACK_TABLE CallbackTable = {
	MngmtFunc,        // mngmtFunc
	FeatureClear,     // featureClear
	FeatureSet,       // featureSet
	NULL,             // configurationGet
	ConfigurationSet, // configurationSet
	DescriptorGet,    // descriptorGet
	NULL,             // descriptorSet
	InterfaceGet,     // interfaceGet
	InterfaceSet,     // interfaceSet
	NULL,             // statusGet
	NULL,             // addressSet
	NULL,             // synchFrameGet
	VendorSpecific    // vendorSpecific
};

STATUS
usbtgtinit(void)
{
	USBTGT_FUNC_INFO FuncInfo;

	FuncInfo.ppFsFuncDescTable = FsFuncDescHdr;
	FuncInfo.ppHsFuncDescTable = HsFuncDescHdr;
	FuncInfo.ppSsFuncDescTable = SsFuncDescHdr;
	FuncInfo.pFuncCallbackTable = &CallbackTable;

	FuncInfo.pCallbackParam = &UsbDev;
	FuncInfo.pFuncSpecific = &UsbDev;

	FuncInfo.pFuncName = "usbTgtDummy";
	FuncInfo.pTcdName = "/usbTgt";
	FuncInfo.uFuncUnit = 0;
	FuncInfo.uTcdUnit = 0;
	FuncInfo.uConfigToBind = 1;
	FuncInfo.pWcidString = NULL;
	FuncInfo.pSubWcidString = NULL;
	FuncInfo.uWcidVc = 0;

	UsbDev.targChannel = usbTgtFuncRegister(&FuncInfo);
	if (UsbDev.targChannel == USBTGT_TARG_CHANNEL_DEAD)
		return ERROR;

	return OK;
}
