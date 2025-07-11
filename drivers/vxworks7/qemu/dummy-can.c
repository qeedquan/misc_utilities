/*

Dummy CAN controller driver

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <vxLib.h>
#include <vxFdtLib.h>
#include <spinLockLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/util/vxbIsrDeferLib.h>
#include <canDevLib.h>

void canDevShow(void);
STATUS canDevSend(CAN_DEV *, CAN_MSG *);
STATUS canDevIoctl(CAN_DEV *, int, caddr_t);

typedef struct {
	CAN_DEV candev;
	VXB_DEV_ID dev;
	void *cookie;
} Ctlr;

LOCAL STATUS
dcanxmit(CAN_DEV *candev, CAN_MSG *msg)
{
	printf("%s(candev=%p, msg=%p)\n", __func__, candev, msg);
	return OK;
}

LOCAL STATUS
dcanioctl(CAN_DEV *candev, int cmd, caddr_t data)
{
	printf("%s(candev=%p, cmd=%#x, data=%p)\n", __func__, candev, cmd, data);
	return ERROR;
}

LOCAL CAN_FUNCS dcannetfuncs = {
	dcanioctl,
	dcanxmit,
	NULL,
	NULL,
};

LOCAL STATUS
dcanprobe(VXB_DEV_ID dev)
{
	return OK;
}

LOCAL STATUS
dcanattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->cookie = canDevConnect(&ctlr->candev, &dcannetfuncs, "Dummy CAN Controller");
	if (!ctlr->cookie)
		goto error;

	vxbDevSoftcSet(dev, ctlr);
	ctlr->dev = dev;
	return OK;

error:
	vxbMemFree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD dcandev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dcanprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dcanattach },
	VXB_DEVMETHOD_END
};

VXB_DRV dcandrv = {
	{ NULL },
	"dcan",
	"Dummy CAN Controller Driver",
	VXB_BUSID_NEXUS,
	0,
	0,
	dcandev,
	NULL,
};

VXB_DRV_DEF(dcandrv)

STATUS
dcandrvadd(void)
{
	STATUS status;

	status = vxbDrvAdd(&dcandrv);
	printf("CAN Device Config Max Number: %d\n", canDevCfgMaxNumber());
	printf("CAN Device Count: %d\n", canDevCount());
	canDevShow();
	return status;
}

void
dcantest(int index)
{
	CAN_DEV *candev;

	candev = canDevGetByIndex(index);
	if (!candev) {
		printf("Cannot get CAN device\n");
		return;
	}

	canDevSend(candev, NULL);
	canDevIoctl(candev, 0, 0);
}

void
dcansocktest(int index)
{
	struct sockaddr_can addr;
	CAN_MSG msg;
	ssize_t nwritten;
	int fd;
	UINT8 opt;

	fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (fd < 0) {
		perror("socket");
		goto out;
	}

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = index;

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		goto out;
	}

	// allow receiving CAN-FD frames
	opt = 1;
	if (setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &opt, sizeof(opt)) < 0)
		perror("setsockopt");

	memset(&msg, 0, sizeof(msg));
	nwritten = write(fd, &msg, sizeof(msg));
	printf("written: %zd\n", nwritten);

out:
	if (fd >= 0)
		close(fd);
}
