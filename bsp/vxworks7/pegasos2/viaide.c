#include "dat.h"
#include "fns.h"

IMPORT UCHAR erfLibInitialized;

// IO Ports
enum {
	Data = 0,
	Error = 1,    // (read)
	Features = 1, // (write)
	Count = 2,    // sector count<7-0>, sector count<15-8>
	Ir = 2,       // interrupt reason (PACKET)
	Sector = 3,   // sector number
	Lbalo = 3,    // LBA<7-0>, LBA<31-24>
	Cyllo = 4,    // cylinder low
	Bytelo = 4,   // byte count low (PACKET)
	Lbamid = 4,   // LBA<15-8>, LBA<39-32>
	Cylhi = 5,    // cylinder high
	Bytehi = 5,   // byte count hi (PACKET)
	Lbahi = 5,    // LBA<23-16>, LBA<47-40>
	Dh = 6,       // Device/Head, LBA<27-24>
	Status = 7,   // (read)
	Command = 7,  // (write)

	As = 2, // Alternate Status (read)
	Dc = 2, // Device Control (write)
};

// Bus Master Registers
enum {
	BM_COMMAND = 0,
	BM_STATUS = 2,
	BM_DTP = 4,
};

// Device Control
enum {
	Nien = 0x02, // (not) interrupt enable
	Srst = 0x04, // software reset
	Hob = 0x80,  // high order bit [sic]

	Drivebit = 4,
};

// Alternate Status Bits
enum {
	ATA_STAT_BUSY = 0x80,
	ATA_STAT_READY = 0x40,
	ATA_STAT_DMAR = 0x20,
	ATA_STAT_SERV = 0x10,
	ATA_STAT_DRQ = 0x08,
	ATA_STAT_ERR = 0x01,
};

// Message IDs
enum {
	VIA_IDE_INIT_MSG = 1,
};

typedef struct {
	int id; // message id
} VIA_IDE_MSG;

typedef struct {
	SATA_DEVICE satadev[4];
	SATA_HOST satahost[2];

	VXB_DEV_ID dev;

	// monitor message queue
	MSG_Q_ID monqueue;

	// io mutex
	SEMAPHORE rwsem[2];

	// ide0 data/cmd; ide1 data/cmd; bmdma
	VXB_RESOURCE *res[5];
	void *reghandle[5];
	VIRT_ADDR regbase[5];

	VXB_RESOURCE *intres;

	VXB_RESOURCE reslist[1];
	VXB_RESOURCE_ADR resadrlist[1];
	VXB_RESOURCE_IRQ resirqlist[1];
	VXB_INTR_ENTRY intrentry[1];
} VIA_IDE_CTLR;

LOCAL const VXB_PCI_DEV_MATCH_ENTRY viaidepci[] = {
	{ 0x0571, 0x1106, NULL },
	{},
};

LOCAL UINT8
csrdat8r(VIA_IDE_CTLR *ctlr, int unit, int addr)
{
	int i;

	i = unit << 1;
	return vxbRead8(ctlr->reghandle[i], (UINT8 *)(ctlr->regbase[i] + addr));
}

LOCAL UINT32
csrdat32r(VIA_IDE_CTLR *ctlr, int unit, int addr)
{
	int i;

	i = unit << 1;
	return vxbRead32(ctlr->reghandle[i], (UINT32 *)(ctlr->regbase[i] + addr));
}

LOCAL void
csrdat8w(VIA_IDE_CTLR *ctlr, int unit, int addr, UINT8 val)
{
	int i;

	i = unit << 1;
	vxbWrite8(ctlr->reghandle[i], (UINT8 *)(ctlr->regbase[i] + addr), val);
}

LOCAL void
csrdat32w(VIA_IDE_CTLR *ctlr, int unit, int addr, UINT32 val)
{
	int i;

	i = unit << 1;
	vxbWrite32(ctlr->reghandle[i], (UINT32 *)(ctlr->regbase[i] + addr), val);
}

LOCAL UINT8
csrcmd8r(VIA_IDE_CTLR *ctlr, int unit, int addr)
{
	int i;

	i = (unit << 1) + 1;
	return vxbRead8(ctlr->reghandle[i], (UINT8 *)(ctlr->regbase[i] + addr));
}

LOCAL UINT8
csrbm8r(VIA_IDE_CTLR *ctlr, int addr)
{
	return vxbRead8(ctlr->reghandle[4], (UINT8 *)(ctlr->regbase[4] + addr));
}

LOCAL void
csrbm8w(VIA_IDE_CTLR *ctlr, int addr, UINT8 val)
{
	return vxbWrite8(ctlr->reghandle[4], (UINT8 *)(ctlr->regbase[4] + addr), val);
}

LOCAL STATUS
statuscheck(VIA_IDE_CTLR *ctlr, int unit, UINT8 mask, UINT8 status)
{
	static const int tries = 10;

	int i, j;

	for (i = 0; i < tries; i++) {
		if ((csrcmd8r(ctlr, unit, As) & mask) == status)
			return OK;

		// delay
		for (j = 0; j < 5; j++)
			csrcmd8r(ctlr, unit, As);
	}
	return ERROR;
}

LOCAL STATUS
issuerw(VIA_IDE_CTLR *ctlr, SATA_HOST *satahost, SATA_DEVICE *satadev, FIS_ATA_REG *fisata, SATA_DATA *satadata, int unit, int drive, int cmdflag)
{
	UINT32 *data;
	UINT8 uselba;
	int numsectors;
	int block;
	int len;
	int i;

	block = 1;
	len = ATA_SECTOR_SIZE;
	uselba = 0;
	if (satadev->okLba48 || satadev->okLba)
		uselba = USE_LBA;

	csrdat8w(ctlr, unit, Count, fisata->fisCmd.fisAtaCmd[13]);
	csrdat8w(ctlr, unit, Count, fisata->fisCmd.fisAtaCmd[12]);
	if (satadev->okLba48) {
		csrdat8w(ctlr, unit, Sector, fisata->fisCmd.fisAtaCmd[8]);
		csrdat8w(ctlr, unit, Sector, fisata->fisCmd.fisAtaCmd[4]);
		csrdat8w(ctlr, unit, Cyllo, fisata->fisCmd.fisAtaCmd[9]);
		csrdat8w(ctlr, unit, Cyllo, fisata->fisCmd.fisAtaCmd[5]);
		csrdat8w(ctlr, unit, Cylhi, fisata->fisCmd.fisAtaCmd[10]);
		csrdat8w(ctlr, unit, Cylhi, fisata->fisCmd.fisAtaCmd[6]);
		csrdat8w(ctlr, unit, Dh, ((drive << Drivebit) & 0xff) | uselba);
	} else {
		csrdat8w(ctlr, unit, Sector, fisata->fisCmd.fisAtaCmd[4]);
		csrdat8w(ctlr, unit, Cyllo, fisata->fisCmd.fisAtaCmd[5]);
		csrdat8w(ctlr, unit, Cylhi, fisata->fisCmd.fisAtaCmd[6]);
		csrdat8w(ctlr, unit, Dh, (fisata->fisCmd.fisAtaCmd[7] & 0xf) | ((drive << Drivebit) & 0xff) | uselba);
	}

	csrdat8w(ctlr, unit, Command, fisata->ataReg.command);
	if (statuscheck(ctlr, unit, ATA_STAT_DRQ | ATA_STAT_BUSY, ATA_STAT_DRQ | !ATA_STAT_BUSY) != OK)
		return ERROR;

	numsectors = (satadata->blkNum * satadata->blkSize) / ATA_SECTOR_SIZE;
	while (numsectors > 0) {
		data = satadata->buffer;

		if (cmdflag & ATA_FLAG_IN_DATA) {
			for (i = 0; i < len; i += 4)
				*data++ = csrdat32r(ctlr, unit, Data);
		} else {
			for (i = 0; i < len; i += 4)
				csrdat32w(ctlr, unit, Data, *data++);
		}

		satadata->buffer = satadata->buffer + len;
		numsectors -= block;
	}

	return OK;
}

LOCAL STATUS
issuecmd(SATA_DEVICE *satadev, FIS_ATA_REG *fisata, SATA_DATA *satadata)
{
	VIA_IDE_CTLR *ctlr;
	SATA_HOST *satahost;
	STATUS status;
	int unit;
	int drive;
	int cmdflag;

	status = OK;
	satahost = satadev->host;
	ctlr = satahost->pCtrlExt;
	unit = satahost->numCtrl;
	drive = satadev->num;
	cmdflag = fisata->fisCmd.fisCmdFlag;

	semTake(&ctlr->rwsem[unit], WAIT_FOREVER);
	if (cmdflag & (ATA_FLAG_SRST_ON | ATA_FLAG_SRST_OFF)) {
		// ignore
	} else if (cmdflag & ATA_FLAG_NON_DATA) {
		csrdat8w(ctlr, unit, Features, fisata->ataReg.feature);
		csrdat8w(ctlr, unit, Sector, fisata->ataReg.sector);
		csrdat8w(ctlr, unit, Lbalo, fisata->ataReg.lbaLow);
		csrdat8w(ctlr, unit, Lbamid, fisata->ataReg.lbaMid);
		csrdat8w(ctlr, unit, Lbahi, fisata->ataReg.lbaHigh);
		csrdat8w(ctlr, unit, Dc, (drive << Drivebit) & 0xff);
	} else if (cmdflag & (ATA_FLAG_IN_DATA | ATA_FLAG_OUT_DATA)) {
		status = issuerw(ctlr, satahost, satadev, fisata, satadata, unit, drive, cmdflag);
	} else {
		kprintf("viaide: unknown command: %#x\n", cmdflag);
		status = ERROR;
	}
	semGive(&ctlr->rwsem[unit]);

	return status;
}

LOCAL void
initxbd(VIA_IDE_CTLR *ctlr)
{
	SATA_DEVICE *satadev;
	SATA_HOST *satahost;
	size_t i, j;
	size_t numsatadev;

	numsatadev = 0;
	for (i = 0; i < NELEMENTS(ctlr->satahost); i++) {
		satahost = &ctlr->satahost[i];
		satahost->ops.cmdIssue = issuecmd;
		satahost->pCtrlExt = ctlr;
		satahost->numCtrl = i;

		if (semMInit(&ctlr->rwsem[i], SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE) != OK) {
			memset(satahost, 0, sizeof(*satahost));
			continue;
		}

		for (j = 0; j < 2; j++) {
			satadev = &ctlr->satadev[numsatadev];
			satadev->host = satahost;
			satadev->attached = TRUE;
			satadev->num = j;
			satadev->logicPortNum = numsatadev;
			satadev->type = ATA_TYPE_ATA;
			satadev->xbdDev.pPort = satadev;

			if (sataAtaNormalInit(satadev) != OK) {
				satadev->attached = FALSE;
				continue;
			}

			// use the simplest io mode
			satadev->rwMode = ATA_RWMODE_PIO;
			satadev->okLba = FALSE;
			satadev->okLba48 = FALSE;
			satadev->okDma = FALSE;
			satadev->pioMode = 0;

			satadev->prdMaxSize = MAX_BYTES_PER_PRD;
			satadev->fisMaxSector = MAX_SECTORS_FISNOMAL;
			if (satadev->okLba48)
				satadev->fisMaxSector = MAX_SECTORS_FISLBA48;

			if (sataXbdDevCreate(satadev, NULL) == NULLDEV) {
				satadev->attached = FALSE;
				continue;
			}

			numsatadev++;

			break;
		}

		break;
	}
}

LOCAL void
viaideint(VXB_DEV_ID dev)
{
	VIA_IDE_CTLR *ctlr;
	unsigned unit;
	UINT8 bm;

	ctlr = vxbDevSoftcGet(dev);
	unit = 0;

	bm = csrbm8r(ctlr, BM_STATUS);
	if (bm & 0x4) {
		// clear PCI level interrupt
		csrbm8w(ctlr, BM_STATUS, bm & ~0x2);
	}

	for (unit = 0; unit < NELEMENTS(ctlr->satahost); unit++) {
		if (ctlr->satahost[unit].pCtrlExt == NULL)
			continue;

		// acknowledge interrupt
		if (!(csrcmd8r(ctlr, unit, As) & ATA_STAT_BUSY))
			csrdat8r(ctlr, unit, Status);
	}
}

LOCAL STATUS
viaidemon(VIA_IDE_CTLR *ctlr)
{
	VIA_IDE_MSG msg;

	while (erfLibInitialized == FALSE)
		taskDelay(sysClkRateGet());

	for (;;) {
		msgQReceive(ctlr->monqueue, (char *)&msg, sizeof(msg), WAIT_FOREVER);

		switch (msg.id) {
		case VIA_IDE_INIT_MSG:
			initxbd(ctlr);
			break;
		}
	}

	return OK;
}

LOCAL void
viaidefree(VIA_IDE_CTLR *ctlr)
{
	size_t i;

	if (!ctlr)
		return;

	for (i = 0; i < NELEMENTS(ctlr->res); i++)
		vxbResourceFree(ctlr->dev, ctlr->res[i]);
	vxbMemFree(ctlr);
}

LOCAL STATUS
viaideprobe(VXB_DEV_ID dev)
{
	return vxbPciDevMatch(dev, viaidepci, NULL);
}

LOCAL STATUS
viaideattach(VXB_DEV_ID dev)
{
	VXB_DEV_ID isadev;
	VT8231_CTLR *isactlr;
	VIA_IDE_CTLR *ctlr;
	VXB_RESOURCE_ADR *resadr;
	VXB_RESOURCE *res;
	VXB_RESOURCE_IRQ *resirq;
	VXB_INTR_ENTRY *intrentry;
	VIA_IDE_MSG msg;
	UINT16 i;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	for (i = 0; i < NELEMENTS(ctlr->res); i++) {
		ctlr->res[i] = vxbResourceAlloc(dev, VXB_RES_IO, i);
		if (!ctlr->res[i])
			goto error;

		resadr = ctlr->res[i]->pRes;
		ctlr->regbase[i] = resadr->virtual;
		ctlr->reghandle[i] = resadr->pHandle;
	}
	vxbDevSoftcSet(dev, ctlr);

	ctlr->monqueue = msgQCreate(128, sizeof(VIA_IDE_MSG), MSG_Q_FIFO);
	if (!ctlr->monqueue)
		goto error;
	if (taskSpawn("viaidemon", 49, 0, 8192, viaidemon, (_Vx_usr_arg_t)ctlr, 0, 0, 0, 0, 0, 0, 0, 0, 0) == TASK_ID_ERROR)
		goto error;

	msg.id = VIA_IDE_INIT_MSG;
	msgQSend(ctlr->monqueue, (char *)&msg, sizeof(msg), NO_WAIT, MSG_PRI_NORMAL);

	isadev = vxbDevAcquireByName("vt8231-isa", 0);
	if (isadev) {
		isactlr = vxbDevSoftcGet(isadev);
		res = &ctlr->reslist[0];
		resirq = &ctlr->resirqlist[0];
		intrentry = &ctlr->intrentry[0];
		res->pRes = resirq;
		res->id = VXB_RES_ID_CREATE(VXB_RES_IRQ, 0);
		resirq->pVxbIntrEntry = intrentry;
		resirq->hVec = 14;
		resirq->lVec = isactlr->intbase + resirq->hVec;
		ctlr->intres = res;

		vxbIntConnect(ctlr->dev, ctlr->intres, viaideint, ctlr->dev);
		vxbIntEnable(ctlr->dev, ctlr->intres);

		vxbDevRelease(isadev);
	}

	return OK;

error:
	viaidefree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD viaidedev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), viaideprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), viaideattach },
	VXB_DEVMETHOD_END,
};

VXB_DRV viaidedrv = {
	{ NULL },
	"via-ide",
	"VIA IDE driver",
	VXB_BUSID_PCI,
	0,
	0,
	viaidedev,
	NULL,
};

VXB_DRV_DEF(viaidedrv)

void
viaidelink(void)
{
}

void
xbdtest(void)
{
	int fd;
	char buf[32];
	ssize_t len;
	ssize_t i;

	fd = open("/ata0", O_RDONLY);
	if (fd < 0)
		return;

	memset(buf, 0, sizeof(buf));
	len = read(fd, buf, sizeof(buf));
	close(fd);

	printf("READ %zd\n", len);
	for (i = 0; i < len; i++)
		printf("BUF: %#x\n", buf[i]);
}

int
xbdinfo(const char *name)
{
	devname_t devname;
	XBD_GEOMETRY geo;
	XBD_INFO info;
	ATA_IDENTIFY_INFO ata;
	int attached;
	int devtype;
	int fd;
	VXB_DEV_ID dev;
	VIA_IDE_CTLR *ctlr;

	dev = vxbDevAcquireByName("via-ide", 0);
	ctlr = vxbDevSoftcGet(dev);

	fd = open(name, O_RDONLY);
	if (fd < 0) {
		printf("Error: %s\n", strerror(errno));
		return -errno;
	}

	memset(&geo, 0, sizeof(geo));
	memset(&info, 0, sizeof(info));
	memset(&ata, 0, sizeof(ata));
	devname[0] = '\0';
	devtype = -1;

	ioctl(fd, XBD_GETGEOMETRY, &geo);
	ioctl(fd, XBD_GETBASENAME, devname);
	attached = ioctl(fd, XBD_TEST);
	ioctl(fd, XBD_GETINFO, &info);
	ioctl(fd, XBD_GET_DEVTYPE, &devtype);
	ioctl(fd, XBD_SATA_PARMSGET, &ata);

	printf("Device Name: %s\n", devname);
	printf("Device Type: %d\n", devtype);
	printf("Attached: %d\n", attached >= 0);
	printf("\n");

	printf("Geometry Info\n");
	printf("Heads: %u\n", geo.heads);
	printf("Cylinders: %llu\n", geo.cylinders);
	printf("Total Blocks: %lld\n", (long long)geo.total_blocks);
	printf("Sectors Per Tracks: %u\n", geo.secs_per_track);
	printf("Block Size: %u\n", geo.blocksize);
	printf("\n");

	printf("ATA Info:\n");
	printf("Serial: %.*s\n", sizeof(ata.serial), ata.serial);
	printf("Revision: %*.s\n", sizeof(ata.rev), ata.rev);
	printf("Model: %*.s\n", sizeof(ata.model), ata.model);
	printf("Cylinders: %d\n", ata.cylinders);
	printf("Heads: %d\n", ata.heads);
	printf("Bytes Per Sector: %d\n", ata.bytesSec);
	printf("Sector Buffer Size (In Sectors): %d\n", ata.size);
	printf("ECC Bytes: %d\n", ata.bytesEcc);
	printf("PIO Mode: %d\n", ata.pioMode);
	printf("DMA Mode: %d\n", ata.dmaMode);
	printf("Capacity 0: %d\n", ata.capacity0);
	printf("Capacity 1: %d\n", ata.capacity1);
	printf("Sectors 0: %d\n", ata.sectors0);
	printf("Sectors 1: %d\n", ata.sectors1);
	printf("Queue Depth: %d\n", ata.queueDepth);

	sataOneDeviceShow(&ctlr->satadev[0]);

	close(fd);
	return 0;
}
