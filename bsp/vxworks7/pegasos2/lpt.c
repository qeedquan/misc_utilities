#include "dat.h"
#include "fns.h"

#define csr8r(c, a) vxbRead8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))))
#define csr8w(c, a, v) vxbWrite8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))), v)

// Registers
enum {
	DATA = 0,
	STAT,
	CTRL,
};

// Control Bits
enum {
	C_STROBE = 0x01,
	C_AUTOFEED = 0x02,
	C_NOINIT = 0x04,
	C_SELECT = 0x08,
	C_ENABLE = 0x10,
	C_INPUT = 0x20,
};

// Status Bits
enum {
	S_NODERR = 0x08,
	S_SELECT = 0x10,
	S_PERR = 0x20,
	S_NOACK = 0x40,
	S_NOBUSY = 0x80,
	S_MASK = 0xb8,
};

void
lptint(VXB_DEV_ID dev)
{
}

LOCAL void *
lptopen(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	return devhdr;
}

LOCAL ssize_t
lptread(void *ctx, char *buf, size_t len)
{
	return 0;
}

LOCAL ssize_t
lptwrite(void *ctx, const char *buf, size_t len)
{
	LPT_CTLR *ctlr;
	ssize_t nwrite;

	ctlr = ctx;
	nwrite = 0;

	semTake(ctlr->semio, WAIT_FOREVER);

	if ((csr8r(ctlr, STAT) & S_MASK) != (S_SELECT | S_NODERR | S_NOBUSY))
		goto error;

	while (nwrite < len) {
		csr8w(ctlr, DATA, *buf);
		buf += 1;
		nwrite += 1;
	}

	if (0) {
	error:
		nwrite = -1;
	}

	semGive(ctlr->semio);

	return nwrite;
}

LOCAL STATUS
lptioctl(void *ctx, int func, int arg)
{
	LPT_CTLR *ctlr;
	STATUS status;

	ctlr = ctx;
	status = OK;

	semTake(ctlr->semio, WAIT_FOREVER);
	switch (func) {
	case LPT_SETCONTROL:
		csr8w(ctlr, CTRL, arg & 0xff);
		break;

	case LPT_GETSTATUS:
		*(int *)arg = csr8r(ctlr, STAT);
		break;

	default:
		errnoSet(S_ioLib_UNKNOWN_REQUEST);
		status = ERROR;
	}
	semGive(ctlr->semio);

	return status;
}

LOCAL void
lptinit(LPT_CTLR *ctlr)
{
	UINT8 reg;

	csr8w(ctlr, CTRL, 0);
	vxbUsDelay(100);

	reg = C_NOINIT | C_SELECT;
	if (ctlr->autofeed)
		reg |= C_AUTOFEED;
	csr8w(ctlr, CTRL, reg);
}

STATUS
lptcreate(LPT_CTLR *ctlr)
{
	VXB_DEV_ID dev;
	char name[MAX_DRV_NAME_LEN];

	ctlr->semsync = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
	ctlr->semio = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
	if (ctlr->semsync == SEM_ID_NULL || ctlr->semio == SEM_ID_NULL)
		goto error;

	dev = ctlr->dev;
	ctlr->iosdrvnum = iosDrvInstall(NULL, NULL, lptopen, NULL, lptread, lptwrite, lptioctl);
	if (ctlr->iosdrvnum == ERROR)
		goto error;

	snprintf(name, sizeof(name), "/lpt%d", vxbDevUnitGet(dev));
	if (iosDevAdd(&ctlr->devhdr, name, ctlr->iosdrvnum) == ERROR)
		goto error;

	lptinit(ctlr);

	return OK;

error:
	return ERROR;
}

void
lptlink(void)
{
}

LOCAL VXB_DRV_METHOD lptdev[] = {
	VXB_DEVMETHOD_END,
};

VXB_DRV lptdrv = {
	{ NULL },
	"lpt",
	"LPT Driver",
	VXB_BUSID_FDT,
	0,
	0,
	lptdev,
	NULL,
};

VXB_DRV_DEF(lptdrv)

void
lpttest(void)
{
	char buf[] = "hello";
	int fd;
	int i;

	fd = open("/lpt0", O_RDWR);
	for (i = 0; i < 1000; i++)
		write(fd, buf, sizeof(buf) - 1);
	close(fd);
}