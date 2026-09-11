/*

The IVSH device provides a way to share memory between different emulated instances
or to the host by allocating a PCI BAR for reading/writing to.
The driver sets it up in a way where the user will use mmap to get the PCI BAR into memory for manipulation.
Multiple programs that uses mmap will have to implement mutual exclusion by themselves.

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <devMemLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>

typedef struct {
	VXB_DEV_ID dev;
	char devname[MAX_DRV_NAME_LEN];

	VXB_RESOURCE *res;
} Ctlr;

LOCAL const VXB_PCI_DEV_MATCH_ENTRY ivshpci[] = {
	{ 0x1110, 0x1af4, NULL },
	{},
};

LOCAL void
ivshfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	if (ctlr->res)
		vxbResourceFree(ctlr->dev, ctlr->res);

	if (ctlr->devname[0])
		devMemUnlink(ctlr->devname);

	vxbMemFree(ctlr);
}

LOCAL int
ivshprobe(VXB_DEV_ID dev)
{
	VXB_PCI_DEV_MATCH_ENTRY *match;
	return vxbPciDevMatch(dev, ivshpci, &match);
}

LOCAL STATUS
ivshattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	VXB_RESOURCE_ADR *resadr;
	MMU_ATTR mmuattr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->res = vxbResourceAlloc(dev, VXB_RES_MEMORY, 2);
	if (ctlr->res == NULL)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	resadr = ctlr->res->pRes;
	mmuattr = MMU_ATTR_SUP_RW | MMU_ATTR_USR_RW | MMU_ATTR_VALID | MMU_ATTR_CACHE_OFF;
	snprintf(ctlr->devname, sizeof(ctlr->devname), "/ivshmem%d", vxbDevUnitGet(dev));
	if (devMemCreate(ctlr->devname, 0, resadr->start, resadr->size, mmuattr) == ERROR)
		goto error;

	return OK;

error:
	ivshfree(ctlr);
	return ERROR;
}

LOCAL STATUS
ivshdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	ivshfree(ctlr);
	return OK;
}

LOCAL STATUS
ivshshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD ivshdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), ivshprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), ivshattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), ivshdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), ivshshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV ivshdrv = {
	{ NULL },
	"ivsh",
	"QEMU IVSH driver",
	VXB_BUSID_PCI,
	0,
	0,
	ivshdev,
	NULL,
};

VXB_DRV_DEF(ivshdrv)

STATUS
ivshdrvadd(void)
{
	return vxbDrvAdd(&ivshdrv);
}

void
ivshdrvtest(void)
{
	struct stat st;
	void *data;
	char *ptr;
	int fd;
	int i;

	data = MAP_FAILED;
	fd = open("/devm/ivshmem0", O_RDWR);
	if (fd < 0) {
		printf("open: %s\n", strerror(errno));
		goto out;
	}

	if (fstat(fd, &st) < 0) {
		printf("fstat: %s\n", strerror(errno));
		goto out;
	}

	data = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		printf("mmap: %s\n", strerror(errno));
		goto out;
	}

	printf("mapped %p %zu\n", data, st.st_size);

	// assume host wrote something here so we can read it back
	ptr = data;
	for (i = 0; i < 5; i++)
		printf("%c", ptr[i]);
	printf("\n");

	// write to the buffer so host can see
	memcpy(ptr, "guested", 7);

out:
	if (fd >= 0)
		close(fd);

	if (data != MAP_FAILED)
		munmap(data, st.st_size);
}
