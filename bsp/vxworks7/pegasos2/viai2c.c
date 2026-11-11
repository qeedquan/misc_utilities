#include "dat.h"
#include "fns.h"

#define csr8r(c, a) vxbRead8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))))
#define csr8w(c, a, v) vxbWrite8(c->reghandle, (UINT8 *)(((char *)(c)->regbase) + ((a))), v)

enum {
	IOBASE = 0x500,
};

enum {
	SMBHSTSTS = 0x0,
	SMBHSTCNT = 0x2,
	SMBHSTCMD = 0x3,
	SMBHSTADD = 0x4,
	SMBHSTDAT0 = 0x5,
	SMBHSTDAT1 = 0x6,
	SMBBLKDAT = 0x7,
	SMBAUXCTL = 0xd,
};

// Transfer method
enum {
	PROT_QUICK = 0,
	PROT_BYTE = 1,
	PROT_BYTE_DATA = 2,
	PROT_WORD_DATA = 3,
	PROT_PROC_CALL = 4,
	PROT_BLOCK_DATA = 5,
	PROT_I2C_BLOCK_READ = 6,
};

typedef struct {
	VXB_DEV_ID dev;

	VXB_RESOURCE *res;
	void *reghandle;
	VIRT_ADDR regbase;

	VXB_RESOURCE reslist[1];
	VXB_RESOURCE_ADR resadrlist[1];

	SEM_ID sem;
} VIA_I2C_CTLR;

LOCAL const VXB_PCI_DEV_MATCH_ENTRY viai2cpci[] = {
	{ 0x8235, 0x1106, NULL },
	{},
};

LOCAL void
viai2cfree(VIA_I2C_CTLR *ctlr)
{
	if (!ctlr)
		return;

	vxbResourceFree(ctlr->dev, ctlr->res);
	semDelete(ctlr->sem);
	vxbMemFree(ctlr);
}

LOCAL STATUS
viai2cprobe(VXB_DEV_ID dev)
{
	return vxbPciDevMatch(dev, viai2cpci, NULL);
}

LOCAL void
viai2cprobechild(VXB_DEV_ID dev, VXB_FDT_DEV *parent)
{
	int offset;
	VXB_DEV_ID cur;
	I2C_DEV_INFO *i2cdev;
	VXB_FDT_DEV *fdtdev;

	offset = parent->offset;
	for (offset = VX_FDT_CHILD(offset); offset > 0; offset = VX_FDT_PEER(offset)) {
		cur = NULL;
		if (vxbDevCreate(&cur) != OK)
			continue;

		i2cdev = vxbMemAlloc(sizeof(*i2cdev));
		if (!i2cdev)
			goto error;

		fdtdev = &i2cdev->vxbFdtDev;
		vxbFdtDevSetup(offset, fdtdev);
		vxbDevNameSet(cur, fdtdev->name, FALSE);

		if (vxbResourceInit(&i2cdev->vxbResList) != OK)
			goto error;

		if (vxbFdtRegGet(&i2cdev->vxbResList, fdtdev) != OK)
			goto error;

		if (vxbFdtIntGet(&i2cdev->vxbResList, fdtdev) != OK)
			goto error;

		vxbDevIvarsSet(cur, i2cdev);
		vxbDevClassSet(cur, VXB_BUSID_FDT);

		vxbDevAdd(dev, cur);

		if (0) {
		error:
			if (i2cdev)
				vxbFdtResFree(&i2cdev->vxbResList);
			vxbMemFree(i2cdev);
			vxbDevDestroy(cur);
		}
	}
}

LOCAL STATUS
viai2cattach(VXB_DEV_ID dev)
{
	VIA_I2C_CTLR *ctlr;
	VXB_RESOURCE_ADR *resadr;
	VXB_FDT_DEV fdtdev;
	char fdtpath[32];

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	if (pgs2pciaddio(dev, &ctlr->reslist[0], &ctlr->resadrlist[0]) != OK)
		goto error;

	ctlr->res = vxbResourceAlloc(dev, VXB_RES_IO, 0);
	if (!ctlr->res)
		goto error;

	ctlr->sem = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
	if (ctlr->sem == SEM_ID_NULL)
		goto error;

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual + IOBASE;
	ctlr->reghandle = resadr->pHandle;

	vxbDevSoftcSet(dev, ctlr);

	// set the smbus io base
	VXB_PCI_BUS_CFG_WRITE(dev, 0x90, 4, IOBASE);

	// enable the smbus
	VXB_PCI_BUS_CFG_WRITE(dev, 0xd2, 1, 1);

	snprintf(fdtpath, sizeof(fdtpath), "/pci/i2c");
	memset(&fdtdev, 0, sizeof(fdtdev));
	fdtdev.offset = vxFdtPathOffset(fdtpath);
	if (fdtdev.offset > 0)
		viai2cprobechild(dev, &fdtdev);

	return OK;

error:
	viai2cfree(ctlr);
	return ERROR;
}

LOCAL VXB_RESOURCE *
viai2cresalloc(VXB_DEV_ID dev, VXB_DEV_ID child, UINT32 id)
{
	I2C_DEV_INFO *devinfo;
	VXB_RESOURCE *res;
	VXB_RESOURCE_ADR *resadr;
	int restype;

	devinfo = vxbDevIvarsGet(child);
	res = vxbResourceFind(&devinfo->vxbResList, id);
	if (!res)
		goto error;

	restype = VXB_RES_TYPE(res->id);
	switch (restype) {
	case VXB_RES_MEMORY:
	case VXB_RES_IO:
		resadr = res->pRes;
		resadr->virtual = resadr->start;
		break;

	case VXB_RES_IRQ:
		if (vxbIntMap(res) != OK)
			goto error;
		break;

	default:
		goto error;
	}

	if (0) {
	error:
		res = NULL;
	}

	return res;
}

LOCAL STATUS
viai2cresfree(VXB_DEV_ID dev, VXB_DEV_ID child, VXB_RESOURCE *res)
{
	I2C_DEV_INFO *devinfo;
	int restype;

	devinfo = vxbDevIvarsGet(child);
	restype = VXB_RES_TYPE(res->id);
	if (restype == VXB_RES_MEMORY || restype == VXB_RES_IO)
		return ERROR;
	return vxbResourceRemove(&devinfo->vxbResList, res);
}

LOCAL VXB_RESOURCE_LIST *
viai2creslistget(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	I2C_DEV_INFO *devinfo;

	devinfo = vxbDevIvarsGet(child);
	if (!devinfo)
		return NULL;
	return &devinfo->vxbResList;
}

LOCAL VXB_FDT_DEV *
viai2cdevget(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	I2C_DEV_INFO *devinfo;

	if (child == NULL)
		return NULL;

	devinfo = vxbDevIvarsGet(child);
	if (!devinfo)
		return NULL;

	return &devinfo->vxbFdtDev;
}

LOCAL STATUS
smbcmd(VIA_I2C_CTLR *ctlr, int op, UINT8 proto, UINT8 addr, UINT8 *cmd, UINT8 *data0, UINT8 *data1, UINT8 *stat)
{
	UINT8 val;

	// set the address
	csr8w(ctlr, SMBHSTADD, (addr << 1) | (op == 'r'));

	// set protocol transfer
	csr8w(ctlr, SMBHSTCNT, proto << 2);

	// clear previous status
	csr8w(ctlr, SMBHSTSTS, 0x1e);

	// start transfer
	csr8w(ctlr, SMBHSTCNT, 0x40 | (proto << 2));

	if (op == 'w') {
		if (cmd)
			csr8w(ctlr, SMBHSTCMD, *cmd);
		if (data0)
			csr8w(ctlr, SMBHSTDAT0, *data0);
		if (data1)
			csr8w(ctlr, SMBHSTDAT1, *data1);
	}

	while ((val = csr8r(ctlr, SMBHSTSTS) & 0x1))
		;

	if (op == 'r') {
		if (cmd)
			*cmd = csr8r(ctlr, SMBHSTCMD);
		if (data0)
			*data0 = csr8r(ctlr, SMBHSTDAT0);
		if (data1)
			*data1 = csr8r(ctlr, SMBHSTDAT1);
	}

	if (stat)
		*stat = val;

	return OK;
}

LOCAL STATUS
smbusread(VIA_I2C_CTLR *ctlr, UINT8 addr, UINT8 *data, UINT32 len)
{
	STATUS status;
	UINT8 stat;
	UINT32 i;

	for (i = 0; i < len; i++) {
		status = smbcmd(ctlr, 'r', PROT_BYTE, addr, NULL, data + i, NULL, &stat);
		if (status != OK)
			break;

		if (stat & 0x1c) {
			status = ERROR;
			break;
		}
	}
	return status;
}

LOCAL STATUS
smbuswrite(VIA_I2C_CTLR *ctlr, UINT8 addr, UINT8 *data, UINT32 len)
{
	STATUS status;
	UINT8 stat;

	switch (len) {
	case 3:
		status = smbcmd(ctlr, 'w', PROT_WORD_DATA, addr, data, data + 1, data + 2, &stat);
		break;
	case 2:
		status = smbcmd(ctlr, 'w', PROT_BYTE_DATA, addr, data, data + 1, NULL, &stat);
		break;
	case 1:
		status = smbcmd(ctlr, 'w', PROT_BYTE, addr, data, NULL, NULL, &stat);
		break;
	default:
		return ERROR;
	}

	if (stat & 0x1c)
		status = ERROR;

	return status;
}

LOCAL STATUS
viai2cxfer(VXB_DEV_ID dev, I2C_MSG *msgs, size_t num)
{
	VIA_I2C_CTLR *ctlr;
	I2C_MSG *msg;
	STATUS status;
	size_t i;

	ctlr = vxbDevSoftcGet(dev);
	status = OK;

	semTake(ctlr->sem, WAIT_FOREVER);
	for (i = 0; i < num; i++) {
		msg = msgs + i;
		if (msg->flags & I2C_M_TEN) {
			status = ERROR;
			break;
		}

		if (msg->flags & I2C_M_RD)
			status = smbusread(ctlr, msg->addr, msg->buf, msg->len);
		else {
			status = smbuswrite(ctlr, msg->addr, msg->buf, msg->len);
			if (status != OK)
				break;

			if (msg->wrTime != 0)
				vxbUsDelay(msg->wrTime);
		}
	}
	semGive(ctlr->sem);

	return status;
}

LOCAL VXB_DRV_METHOD viai2cdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), viai2cprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), viai2cattach },
	{ VXB_DEVMETHOD_CALL(vxbResourceAlloc), (FUNCPTR)viai2cresalloc },
	{ VXB_DEVMETHOD_CALL(vxbResourceFree), (FUNCPTR)viai2cresfree },
	{ VXB_DEVMETHOD_CALL(vxbResourceListGet), (FUNCPTR)viai2creslistget },
	{ VXB_DEVMETHOD_CALL(vxbI2cXfer), (FUNCPTR)viai2cxfer },
	{ VXB_DEVMETHOD_CALL(vxbFdtDevGet), (FUNCPTR)viai2cdevget },
	VXB_DEVMETHOD_END,
};

VXB_DRV viai2cdrv = {
	{ NULL },
	"via-i2c",
	"VIA VT8235 I2C Controller",
	VXB_BUSID_PCI,
	0,
	0,
	viai2cdev,
	NULL,
};

VXB_DRV_DEF(viai2cdrv)

void
viai2clink(void)
{
}
