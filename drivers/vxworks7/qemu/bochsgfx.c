/*

Bochs display driver, this driver is very loosely based on fbdev driver framework.
The main difference is pixels->vram happens on with a copy, rather allowing direct mapping to vram.

This can only run in kernel space if there is no fbdev driver installed, since proper fbdev drivers
install an ioctl table so framebufer ops can be passed through properly in RTPs.
Otherwise, ioctl not registered gets dropped in RTPs rather than a passthrough.

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ioLib.h>
#include <iosLib.h>
#include <semLib.h>
#include <errnoLib.h>
#include <fbdev.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>

enum {
	VBEXRES = 0x1,
	VBEYRES = 0x2,
	VBEBPP = 0x3,
	VBEENB = 0x4,
};

typedef struct {
	DEV_HDR devhdr;
	VXB_DEV_ID dev;

	VXB_RESOURCE *vram;
	VXB_RESOURCE *mmio;
	int iosdrvnum;

	SEM_ID sem;

	VIRT_ADDR mmiobase;
	void *mmiohandle;
	size_t mmiosize;

	VIRT_ADDR vrambase;
	void *vramhandle;
	size_t vramsize;

	VIRT_ADDR vbebase;

	uint32_t *pixels;
	int width;
	int height;
	int bpp;
	int refreshrate;
} Ctlr;

#define vber(c, a) vxbRead16(c->mmiohandle, (UINT16 *)(((char *)(c)->vbebase) + ((a) << 1)))
#define vbew(c, a, v) vxbWrite16(c->mmiohandle, (UINT16 *)(((char *)(c)->vbebase) + ((a) << 1)), v)

LOCAL const VXB_PCI_DEV_MATCH_ENTRY bochsgfxpci[] = {
	{ 0x1111, 0x1234, NULL },
	{},
};

LOCAL const int resolutions[][2] = {
	{ 640, 480 },
	{ 800, 600 },
	{ 1024, 768 },
	{ 1280, 720 },
	{ 1280, 800 },
	{ 1280, 1024 },
	{ 1440, 900 },
	{ 1600, 900 },
	{ 1680, 1050 },
	{ 1920, 1080 },
};

LOCAL void
show(Ctlr *ctlr)
{
	static const char *vberegs[] = {
		"ID",
		"XRES",
		"YRES",
		"BPP",
		"ENABLE",
		"BANK",
		"VIRT_WIDTH",
		"VIRT_HEIGHT",
		"X_OFFSET",
		"Y_OFFSET",
	};

	size_t i;

	printf("VRAM: %p | %zu bytes\n", ctlr->vrambase, ctlr->vramsize);
	printf("MMIO: %p | %zu bytes\n", ctlr->mmiobase, ctlr->mmiosize);
	for (i = 0; i < NELEMENTS(vberegs); i++) {
		printf("%-12s: %#04" PRIx32 "\n", vberegs[i], vber(ctlr, i));
	}
}

LOCAL int
getmode(const char *str)
{
	int width, height, bpp, refreshrate;
	size_t i;

	if (sscanf(str, "%dx%d-%d@%d", &width, &height, &bpp, &refreshrate) != 4)
		return -1;

	if (bpp != 32 || refreshrate != 60)
		return -1;

	for (i = 0; i < NELEMENTS(resolutions); i++) {
		if (width == resolutions[i][0] && height == resolutions[i][1])
			return i;
	}
	return -1;
}

LOCAL STATUS
setmode(Ctlr *ctlr, unsigned mode)
{
	unsigned width, height, bpp;

	if (mode >= NELEMENTS(resolutions))
		return ERROR;

	width = resolutions[mode][0];
	height = resolutions[mode][1];
	bpp = 32;
	if ((width * height * (bpp / 8) * 2) >= ctlr->vramsize) {
		return ERROR;
	}

	ctlr->pixels = (void *)ctlr->vrambase;
	ctlr->width = width;
	ctlr->height = height;
	ctlr->bpp = bpp;
	ctlr->refreshrate = 60;

	vbew(ctlr, VBEBPP, ctlr->bpp);
	vbew(ctlr, VBEXRES, ctlr->width);
	vbew(ctlr, VBEYRES, ctlr->height);
	vbew(ctlr, VBEENB, 1);

	return OK;
}

LOCAL void
clear(Ctlr *ctlr)
{
	memset(ctlr->pixels, 0, ctlr->width * ctlr->height * (ctlr->bpp / 8));
}

LOCAL void
bochsgfxfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	if (ctlr->sem)
		semDelete(ctlr->sem);

	if (ctlr->vram)
		vxbResourceFree(ctlr->dev, ctlr->vram);

	if (ctlr->mmio)
		vxbResourceFree(ctlr->dev, ctlr->mmio);

	if (ctlr->iosdrvnum != ERROR)
		iosDrvRemove(ctlr->iosdrvnum, TRUE);

	vxbMemFree(ctlr);
}

LOCAL void *
bochsgfxopen(DEV_HDR *devhdr, const char *name, int flags, int mode)
{
	return devhdr;

	(void)name;
	(void)flags;
	(void)mode;
}

LOCAL ssize_t
bochsgfxread(void *ctx, char *buf, size_t len)
{
	return 0;

	(void)ctx;
	(void)buf;
	(void)len;
}

LOCAL ssize_t
bochsgfxwrite(void *ctx, const char *buf, size_t len)
{
	return 0;

	(void)ctx;
	(void)buf;
	(void)len;
}

LOCAL STATUS
bochsgfxioctl(void *ctx, int func, _Vx_ioctl_arg_t arg)
{
	static const FB_PIXEL_FORMAT pixelformat = {
		.flags = FB_PIX_NATIVE,
		.alphaBits = 8,
		.redBits = 8,
		.greenBits = 8,
		.blueBits = 8,
	};

	Ctlr *ctlr;
	FB_IOCTL_ARG *fbi;
	int mode;
	size_t i;
	STATUS r;

	r = OK;
	ctlr = ctx;
	fbi = (FB_IOCTL_ARG *)arg;

	semTake(ctlr->sem, WAIT_FOREVER);
	switch (func) {
	case FB_IOCTL_GET_VIDEO_MODE:
		snprintf(fbi->getVideoMode.pBuf, FB_MAX_VIDEO_MODE_LEN, "%dx%d-%d@%d",
		    ctlr->width, ctlr->height, ctlr->bpp, ctlr->refreshrate);
		break;

	case FB_IOCTL_SET_VIDEO_MODE:
		mode = getmode(fbi->setVideoMode.videoMode);
		if (mode < 0) {
			errnoSet(EINVAL);
			r = ERROR;
			break;
		}
		break;

	case FB_IOCTL_GET_FB_INFO:
		memset(&fbi->getFbInfo, 0, sizeof(fbi->getFbInfo));
		fbi->getFbInfo.width = ctlr->width;
		fbi->getFbInfo.height = ctlr->height;
		fbi->getFbInfo.bpp = ctlr->bpp;
		fbi->getFbInfo.stride = ctlr->width * (ctlr->bpp / 8);
		fbi->getFbInfo.buffers = 2;
		fbi->getFbInfo.pixelFormat = pixelformat;
		break;

	case FB_IOCTL_GET_CONFIGS:
		for (i = 0; i < FB_MAX_CONFIGS && i < NELEMENTS(resolutions); i++) {
			fbi->getConfigs.pConfigs[i].id = i + 1;
			fbi->getConfigs.pConfigs[i].pixelFormat = pixelformat;
		}
		break;

	case FB_IOCTL_SET_FB:
		memcpy(ctlr->pixels, fbi->setFb.pFb, ctlr->width * ctlr->height * (ctlr->bpp / 8));
		break;

	case FB_IOCTL_CLEAR_SCREEN:
		clear(ctlr);
		break;

	case FB_IOCTL_DEV_SHOW:
		show(ctlr);
		break;

	default:
		errnoSet(ENOTSUP);
		r = ERROR;
		break;
	}
	semGive(ctlr->sem);

	return r;
}

LOCAL int
bochsgfxclose(void *ctx)
{
	return OK;

	(void)ctx;
}

LOCAL int
bochsgfxprobe(VXB_DEV_ID dev)
{
	VXB_PCI_DEV_MATCH_ENTRY *match;
	return vxbPciDevMatch(dev, bochsgfxpci, &match);
}

LOCAL STATUS
bochsgfxattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	char name[MAX_DRV_NAME_LEN];
	VXB_RESOURCE_ADR *resadr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->sem = semBCreate(SEM_Q_PRIORITY, SEM_FULL);
	ctlr->vram = vxbResourceAlloc(dev, VXB_RES_MEMORY, 0);
	ctlr->mmio = vxbResourceAlloc(dev, VXB_RES_MEMORY, 2);
	ctlr->iosdrvnum = iosDrvInstall(NULL, NULL, bochsgfxopen, bochsgfxclose, bochsgfxread, bochsgfxwrite, bochsgfxioctl);
	if (ctlr->sem == SEM_ID_NULL || ctlr->vram == NULL || ctlr->mmio == NULL || ctlr->iosdrvnum == ERROR)
		goto error;

	snprintf(name, sizeof(name), "/bochsgfx/%d", vxbDevUnitGet(dev));
	if (iosDevAdd(&ctlr->devhdr, name, ctlr->iosdrvnum) == ERROR)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	resadr = ctlr->vram->pRes;
	ctlr->vrambase = resadr->virtual;
	ctlr->vramhandle = resadr->pHandle;
	ctlr->vramsize = resadr->size;

	resadr = ctlr->mmio->pRes;
	ctlr->mmiobase = resadr->virtual;
	ctlr->mmiohandle = resadr->pHandle;
	ctlr->mmiosize = resadr->size;

	ctlr->vbebase = ctlr->mmiobase + 0x500;
	setmode(ctlr, 2);

	return OK;

error:
	bochsgfxfree(ctlr);
	return ERROR;
}

LOCAL STATUS
bochsgfxdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	bochsgfxfree(ctlr);
	return OK;
}

LOCAL STATUS
bochsgfxshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD bochsgfxdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), bochsgfxprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), bochsgfxattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), bochsgfxdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), bochsgfxshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV bochsgfxdrv = {
	{ NULL },
	"bochsgfx",
	"QEMU Bochs display driver",
	VXB_BUSID_PCI,
	0,
	0,
	bochsgfxdev,
	NULL,
};

VXB_DRV_DEF(bochsgfxdrv)

STATUS
bochsgfxdrvadd(void)
{
	return vxbDrvAdd(&bochsgfxdrv);
}

typedef struct {
	int width;
	int height;
	uint32_t *pixels;
} FB;

LOCAL int
fbinit(FB *fb, int width, int height)
{
	fb->width = width;
	fb->height = height;
	fb->pixels = calloc(1, width * height * sizeof(*fb->pixels));
	if (!fb->pixels)
		return -ENOMEM;
	return 0;
}

LOCAL void
fbfree(FB *fb)
{
	if (!fb)
		return;
	free(fb->pixels);
}

LOCAL void
fbclear(FB *fb, uint32_t color)
{
	int x, y;

	for (y = 0; y < fb->height; y++) {
		for (x = 0; x < fb->width; x++) {
			fb->pixels[y * fb->width + x] = color;
		}
	}
}

LOCAL void
fbflush(FB *fb, int fd)
{
	FB_IOCTL_ARG fbi;

	fbi.setFb.pFb = fb->pixels;
	fbi.setFb.when = 0;
	ioctl(fd, FB_IOCTL_SET_FB, &fbi);
}

void
bochsgfxdrvtest(void)
{
	FB_IOCTL_ARG fbi;
	FB fb[1];
	int fd;

	memset(fb, 0, sizeof(*fb));
	fd = open("/bochsgfx/0", O_RDWR);
	if (fd < 0) {
		printf("failed to open device: %s\n", strerror(errno));
		goto out;
	}

	ioctl(fd, FB_IOCTL_DEV_SHOW, NULL);
	ioctl(fd, FB_IOCTL_GET_FB_INFO, &fbi);
	if (fbinit(fb, fbi.getFbInfo.width, fbi.getFbInfo.height) < 0) {
		printf("failed to init framebuffer\n");
		goto out;
	}

	fbclear(fb, 0x532B1DFF);
	fbflush(fb, fd);

out:
	fbfree(fb);
	if (fd >= 0)
		close(fd);
}
