/*

Dummy Camera driver to test the Camera API

DTS:

dummy_camera: dummy_camera@0 {
    compatible = "dummy_camera";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <cameraLibCore.h>

typedef struct {
	VXB_DEV_ID dev;
	CAMERA_DEV cameradev;
	CAMERA_DEV sensordev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dcamerafdt[] = {
	{ "dummy_camera", NULL },
	{ NULL },
};

LOCAL void
dcamerafree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dcameraprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dcamerafdt, NULL);
}

LOCAL STATUS
dcameraopen(Ctlr *ctlr)
{
	printf("%s(ctlr=%p)\n", __func__, ctlr);
	return OK;
}

LOCAL STATUS
dcameraclose(Ctlr *ctlr)
{
	printf("%s(ctlr=%p)\n", __func__, ctlr);
	return OK;
}

LOCAL STATUS
dcameraioctl(Ctlr *ctlr, CAMERA_IO_CTRL func, void *arg)
{
	printf("%s(ctlr=%p, func=%#x, arg=%p)\n", __func__, ctlr, func, arg);
	return OK;
}

LOCAL STATUS
dcameraattach(VXB_DEV_ID dev)
{
	CAMERA_DEV *cameradev, *sensordev;
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;

	cameradev = &ctlr->cameradev;
	cameradev->open = dcameraopen;
	cameradev->close = dcameraclose;
	cameradev->ioctl = dcameraioctl;
	cameradev->extension = ctlr;
	cameradev->devInfo.pDev = dev;
	cameradev->devInfo.portIndex = 0;
	snprintf(cameradev->devInfo.name, CAMERA_DEV_NAME_LEN, "dummy_camera");

	sensordev = &ctlr->sensordev;
	memcpy(sensordev, cameradev, sizeof(*cameradev));
	snprintf(sensordev->devInfo.name, CAMERA_DEV_NAME_LEN, "dummy_camera_sensor");

	vxbDevSoftcSet(dev, ctlr);

	if (cameraCoreSensorRegister(sensordev) != OK)
		goto error;

	if (cameraCoreControllerRegister(cameradev) != OK)
		goto error;

	return OK;

error:
	dcamerafree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD dcameradev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dcameraprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dcameraattach },
	VXB_DEVMETHOD_END,
};

VXB_DRV dcameradrv = {
	{ NULL },
	"dummy_camera",
	"Dummy Camera driver",
	VXB_BUSID_FDT,
	0,
	0,
	dcameradev,
	NULL,
};

VXB_DRV_DEF(dcameradrv)

STATUS
dcameradrvadd(void)
{
	return vxbDrvAdd(&dcameradrv);
}

void
dcameratest(void)
{
	CAMERA_IOCTL_ARG cio;
	int fd;

	fd = open(CAMERA_DEFAULT_DEV, O_RDWR, 0);
	if (fd < 0) {
		printf("%s: %s\n", CAMERA_DEFAULT_DEV, strerror(errno));
		return;
	}

	memset(&cio, 0, sizeof(cio));
	ioctl(fd, CAMERA_FORMAT_SET, &cio);
	ioctl(fd, CAMERA_REQ_BUFS, &cio);
	ioctl(fd, CAMERA_STREAM_ON, NULL);
	ioctl(fd, CAMERA_STREAM_OFF, NULL);

	close(fd);
}
