/*

Dummy END driver

DTS:

dummy_end@0 {
    compatible = "dummy-end";

    phy-handle = <&dummy_mii>;

    // ethernet chip phy
    dummy_mii: ethernet-phy@0 {
        // unused, just a description string; the matching happens in the phy probe
        compatible = "dummy-mii";

        #address-cells = <1>;
        #size-cells = <0>;

        // specify the address of the phy in mii bus, if this address is wrong, the mii can return bad data
        reg = <13>;
    };
};

If this is loaded as a DKM, only the MUX layer loads the driver (muxShow/etc work).
The IPNET layer won't automatically register the driver for us (ifconfig/etc won't work).
Need to do it manually via ipcom_drv_eth_init in that case.

In a regular ethernet device, the link speed is gotten from reading the MII register

The MII registers are standardized:
https://kb.segger.com/PHY

MII reads/writes are usually done via specialized registers for a SOC;
You specify the MII register to read/write to in a MDIO register, kick it off and wait for the result.

If the MII read is not working, expect to get 0xffff return values.

There is a MII bus monitor task in VxWorks by default that queries the MII registers every few seconds.
If there is a link state change, it will call miiMediaUpdate.

If there is no PHY driver for an END device, it uses a genericPhy driver as a fallback.
This usually works since the standard MII auto-negotiation/speed is standardized,
but if the hardware has electrical characteristics/clock delays/etc,
then it might not since the PHY will need to be configured via their custom registers.

RMII and GMII are not compatible, some drivers only support GMII and if one tries to use it in RMII,
it will exhibit behavior such as TX interrupts/polling working but RX interupts/polling does not work.
The driver will have to configure some specific registers to use RMII mode to make it work.

RMII - 10/100 MBPS
GMII - 1 GBIT
RGMII - 10/100 MBPS and 1 GBIT
SGMII - 10/100 MBPS and 1/2.5 GBIT
QSGMII - Like SGMII but uses alot less signal lines
PSGMII - Like QSGMII but operates at 6.25 GBIT
XGMII - 10 GBIT

*/

#define END_MACROS
#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <subsys/int/vxbIntLib.h>
#include <etherMultiLib.h>
#include <end.h>
#include <endLib.h>
#include <endMedia.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <vxbEndUtil.h>
#include <miiBus.h>
#include <muxLib.h>
#include <endMedia.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <netLib.h>
#include <etherMultiLib.h>
#include <ipcom_drv_eth.h>

IMPORT FUNCPTR _func_m2PollStatsIfPoll;

#define END_HADDR(end) ((end)->mib2Tbl.ifPhysAddress.phyAddress)
#define END_HADDR_LEN(end) ((end)->mib2Tbl.ifPhysAddress.addrLength)

LOCAL int dendunit;

typedef struct {
	// must be first field
	END_OBJ endobj;

	VXB_DEV_ID dev;
	VXB_DEV_ID miidev;

	void *muxdevcookie;
	int unit;

	// PHY identification
	UINT16 phyoui[2];
	// PHY register
	UINT8 phyaddr;

	// max mtu
	int mtu;

	// mii status
	UINT32 mediaactive;
	UINT32 mediastatus;

	// capabilities and stats
	END_CAPABILITIES endcaps;
	END_IFDRVCONF endstats;

	// job queue for rx
	JOB_QUEUE_ID jobqueue;
	QJOB rxjob;

	// ethernet address
	UCHAR enetaddr[6];
	// broadcast address
	UCHAR brdaddr[6];
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dendfdt[] = {
	{ "dummy-end", NULL },
	{ NULL },
};

LOCAL STATUS
dendsetmiidev(VXB_DEV_ID dev, VXB_DEV_ID miidev, INT32 phyaddr)
{
	Ctlr *ctlr;
	MII_DRV_CTRL *miictlr;

	printf("%s(phyaddr=%#x)\n", __func__, phyaddr);

	ctlr = vxbDevSoftcGet(dev);
	ctlr->miidev = miidev;

	miictlr = vxbDevSoftcGet(miidev);
	miictlr->pEndInst = dev;

	miiBusDevInstConnect(dev);
	return OK;
}

LOCAL void
dendrxhandle(void *arg)
{
	QJOB *job;

	job = arg;
	for (;;) {
		sleep(1);
	}
}

LOCAL STATUS
dendstart(END_OBJ *end)
{
	Ctlr *ctlr;

	if (end->flags & IFF_UP)
		return OK;

	ctlr = (Ctlr *)end;
	ctlr->jobqueue = netJobQueueId;
	ctlr->rxjob.func = dendrxhandle;
	END_FLAGS_SET(end, IFF_UP | IFF_RUNNING);

	// tell the MII layer we are online
	// this will be polled continuously for changes by a MII bus monitor task
	ctlr->mediaactive = IFM_ETHER | IFM_NONE;
	ctlr->mediastatus = IFM_AVALID;

	return OK;
}

LOCAL STATUS
dendstop(END_OBJ *end)
{
	if (end->flags & IFF_UP)
		return OK;

	END_FLAGS_CLR(end, IFF_UP | IFF_RUNNING);
	return OK;
}

LOCAL STATUS
dendunload(END_OBJ *end)
{
	if (end->flags & IFF_UP)
		return ERROR;

	return OK;
}

LOCAL int
dendioctl(END_OBJ *end, int cmd, caddr_t data)
{
	Ctlr *ctlr;
	END_CAPABILITIES *hwcaps;
	END_MEDIA *media;
	END_RCVJOBQ_INFO *qinfo;
	int status;
	int nqs;
	long value;

	ctlr = (Ctlr *)end;
	status = 0;
	switch (cmd) {
	case EIOCSADDR: // set MAC address
		memcpy(ctlr->enetaddr, data, sizeof(ctlr->enetaddr));
		memcpy(END_HADDR(end), data, END_HADDR_LEN(end));
		break;

	case EIOCGADDR: // get MAC address
		if (data == NULL) {
			status = EINVAL;
			break;
		}
		memcpy(data, END_HADDR(end), END_HADDR_LEN(end));
		break;

	case EIOCSFLAGS: // set/clear flags
		value = (long)data;
		if (value < 0) {
			value = -value;
			value -= 1;
			END_FLAGS_CLR(end, value);
		} else
			END_FLAGS_SET(end, value);
		break;

	case EIOCGFLAGS: // get flags
		if (data == NULL) {
			status = EINVAL;
			break;
		}
		*(INT32 *)data = END_FLAGS_GET(end);
		break;

	case EIOCPOLLSTART: // begin polled operation
		break;

	case EIOCPOLLSTOP: // end polled operation
		break;

	case EIOCGMIB2233:
	case EIOCGMIB2:
		break;

	case EIOCGPOLLCONF:
		break;

	case EIOCGPOLLSTATS:
		break;

	case EIOCGMEDIALIST:
		break;

	case EIOCMULTIADD:
		break;

	case EIOCMULTIDEL:
		break;

	case EIOCMULTIGET:
		break;

	case EIOCGIFMEDIA:
		media = (END_MEDIA *)data;
		if (!media) {
			status = EINVAL;
			break;
		}
		media->endMediaActive = ctlr->mediaactive;
		media->endMediaStatus = ctlr->mediastatus;
		break;

	case EIOCSIFMEDIA:
		break;

	case EIOCGIFCAP: // get hardware cap
		hwcaps = (END_CAPABILITIES *)data;
		if (!hwcaps) {
			status = EINVAL;
			break;
		}
		hwcaps->csum_flags_tx = ctlr->endcaps.csum_flags_tx;
		hwcaps->csum_flags_rx = ctlr->endcaps.csum_flags_rx;
		hwcaps->cap_available = ctlr->endcaps.cap_available;
		hwcaps->cap_enabled = ctlr->endcaps.cap_enabled;
		break;

	case EIOCSIFCAP: // set hardware cap
		status = ENOTSUP;
		break;

	case EIOCGIFMTU: // get the MTU
		if (data == NULL) {
			status = EINVAL;
			break;
		}
		*(int *)data = end->mib2Tbl.ifMtu;
		break;

	case EIOCSIFMTU: // set the MTU
		value = (INT32)data;
		if (value < 1 || value > ctlr->mtu) {
			status = EINVAL;
			break;
		}
		end->mib2Tbl.ifMtu = value;
		if (end->pMib2Tbl != NULL)
			end->pMib2Tbl->m2Data.mibIfTbl.ifMtu = value;
		break;

	case EIOCGRCVJOBQ:
		qinfo = (END_RCVJOBQ_INFO *)data;
		if (qinfo == NULL) {
			status = EINVAL;
			break;
		}
		nqs = qinfo->numRcvJobQs;
		qinfo->numRcvJobQs = 1;
		if (nqs < 1)
			status = ENOSPC;
		else
			qinfo->qIds[0] = ctlr->jobqueue;
		break;

	case EIOCSRCVJOBQ:
		qinfo = (END_RCVJOBQ_INFO *)data;
		if (qinfo == NULL) {
			status = EINVAL;
			break;
		}

		nqs = qinfo->numRcvJobQs;
		if (nqs > 1)
			status = ENOSPC;
		else
			ctlr->jobqueue = qinfo->qIds[0];
		break;

	case EIOCSVIDFILTER:
		break;

	case EIOCCVIDFILTER:
		break;

	case EIOCVLANGET:
		break;

	case EIOCGMWIDTH:
		break;

	case EIOCGPOLLSTATE:
		break;

	case EIOCGQBU:
		break;

	case EIOCSQBU:
		break;

#if defined(_WRS_KERNEL) && defined(_WRS_CONFIG_TSN_CONFIG)
	case EIOCGTGQBV:
		break;

	case EIOCSTGQBV:
		break;

	case EIOCGCBS:
		break;

	case EIOCSCBS:
		break;
#endif

	case EIOCGNPT: // specify if NPT style drivers are supported
		status = ENOTSUP;
		break;

	case EIOCGSTYLE: // specify if END style drivers are supported
		break;

	default:
		printf("%s: unhandled command %#x\n", __func__, cmd);
		status = EINVAL;
		break;
	}
	return status;
}

LOCAL STATUS
dendsend(END_OBJ *end, M_BLK_ID mblk)
{
	// update stats
	END_ERR_ADD(end, MIB2_OUT_UCAST, +1);

	return OK;
}

LOCAL STATUS
dendmcastaddradd(END_OBJ *end, char *addr)
{
	return OK;
}

LOCAL STATUS
dendpollsend(END_OBJ *end, M_BLK_ID mblk)
{
	return OK;
}

LOCAL STATUS
dendpollreceive(END_OBJ *end, M_BLK_ID mblk)
{
	return OK;
}

LOCAL STATUS
dendmcastaddrdel(END_OBJ *end, char *addr)
{
	return OK;
}

LOCAL STATUS
dendmcastaddrget(END_OBJ *end, MULTI_TABLE *table)
{
	return OK;
}

LOCAL END_OBJ *
dendload(char *loadstr, void *arg)
{
	static NET_FUNCS netfuncs = {
		dendstart,
		dendstop,
		dendunload,
		dendioctl,
		dendsend,
		dendmcastaddradd,
		dendmcastaddrdel,
		dendmcastaddrget,
		dendpollsend,
		dendpollreceive,
		endEtherAddressForm,
		endEtherPacketDataGet,
		endEtherPacketAddrGet,
	};

	VXB_DEV_ID dev;
	Ctlr *ctlr;
	int unit;

	dev = arg;
	ctlr = vxbDevSoftcGet(dev);

	if (!loadstr)
		return NULL;

	if (!loadstr[0]) {
		strcpy(loadstr, "dend");
		return NULL;
	}

	unit = atoi(loadstr);
	ctlr->mtu = 1500;
	memset(ctlr->brdaddr, 0xff, sizeof(ctlr->brdaddr));
	ctlr->enetaddr[0] = 0xde;
	ctlr->enetaddr[1] = 0xad;
	ctlr->enetaddr[2] = 0xbe;
	ctlr->enetaddr[3] = 0xef;
	ctlr->enetaddr[4] = 0xf0;
	ctlr->enetaddr[5] = 0x0d;

	strcpy(loadstr, "dend");
	if (END_OBJ_INIT(&ctlr->endobj, NULL, "dend", unit, &netfuncs, "DUMMY END Driver.") != OK)
		return NULL;

	endM2Init(&ctlr->endobj, M2_ifType_ethernet_csmacd,
	    ctlr->enetaddr, ETHER_ADDR_LEN, ETHERMTU, 100000000,
	    IFF_NOTRAILERS | IFF_SIMPLEX | IFF_MULTICAST | IFF_BROADCAST);

	if (endPoolCreate(3, &ctlr->endobj.pNetPool) == ERROR)
		return NULL;

	ctlr->endstats.ifPollInterval = sysClkRateGet();
	ctlr->endstats.ifEndObj = &ctlr->endobj;
	ctlr->endstats.ifWatchdog = NULL;
	ctlr->endstats.ifValidCounters = END_IFINUCASTPKTS_VALID |
	                                 END_IFINMULTICASTPKTS_VALID | END_IFINBROADCASTPKTS_VALID |
	                                 END_IFINOCTETS_VALID | END_IFINERRORS_VALID |
	                                 END_IFINDISCARDS_VALID | END_IFOUTUCASTPKTS_VALID |
	                                 END_IFOUTMULTICASTPKTS_VALID |
	                                 END_IFOUTBROADCASTPKTS_VALID | END_IFOUTOCTETS_VALID |
	                                 END_IFOUTERRORS_VALID;

	ctlr->endcaps.cap_available = IFCAP_VLAN_MTU;
	ctlr->endcaps.cap_enabled = IFCAP_VLAN_MTU;

	return &ctlr->endobj;
}

LOCAL void
muxconnect(VXB_DEV_ID dev, void *unused)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);

	ctlr->muxdevcookie = muxDevLoad(ctlr->unit, dendload, "", TRUE, dev);
	if (ctlr->muxdevcookie)
		muxDevStart(ctlr->muxdevcookie);

	if (_func_m2PollStatsIfPoll)
		endPollStatsInit(ctlr->muxdevcookie, _func_m2PollStatsIfPoll);
}

LOCAL void
dendfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dendprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dendfdt, NULL);
}

LOCAL STATUS
dendattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	printf("%s(dev=%p)\n", __func__, dev);

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	ctlr->unit = dendunit++;
	ctlr->phyaddr = 13;
	ctlr->phyoui[0] = 0xface;
	ctlr->phyoui[1] = 0xd00d;

	// attach the phy driver
	if (phyAttach(dev) != OK) {
		printf("%s: Failed to attach phy\n", __func__);
		goto error;
	}

	// set up the mii
	if (miiFind(dev, dendsetmiidev) != OK) {
		printf("%s: Failed to find MII\n", __func__);
		goto error;
	}

	muxconnect(dev, NULL);

	return OK;

error:
	dendfree(ctlr);
	return ERROR;
}

LOCAL STATUS
denddetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dendfree(ctlr);
	return OK;
}

LOCAL STATUS
dendshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_FDT_DEV *
denddevinfo(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	return vxbDevIvarsGet(child);
}

VXB_RESOURCE *
dendresalloc(VXB_DEV_ID dev, VXB_DEV_ID child, UINT32 id)
{
	VXB_DEV_ID parent;

	if (!dev || !child)
		return NULL;

	parent = vxbDevParent(dev);
	return VXB_RESOURCE_ALLOC(parent, child, id);
}

STATUS
dendresfree(VXB_DEV_ID dev, VXB_DEV_ID child, VXB_RESOURCE *res)
{
	VXB_DEV_ID parent;

	if (!dev || !child)
		return NULL;

	parent = vxbDevParent(dev);
	return VXB_RESOURCE_FREE(parent, child, res);
}

VXB_RESOURCE_LIST *
dendreslistget(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	VXB_DEV_ID parent;

	if (!dev || !child)
		return NULL;

	parent = vxbDevParent(dev);
	return VXB_RESOURCE_LIST_GET(parent, child);
}

LOCAL STATUS
dendphyread(VXB_DEV_ID parent, UINT8 phyaddr, UINT8 regaddr, UINT16 *data)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(parent);
	*data = 0xffff;
	if (ctlr->phyaddr != phyaddr)
		return OK;

	switch (regaddr) {
	case MII_STAT_REG: // status
		*data = MII_SR_LINK_STATUS |
		        MII_SR_EXT_STS |
		        MII_SR_TX_FULL_DPX |
		        MII_SR_10T_FULL_DPX |
		        MII_SR_AUTO_SEL;
		break;

	case MII_PHY_ID1_REG: // physical id 1
		*data = ctlr->phyoui[0];
		break;

	case MII_PHY_ID2_REG: // physical id 2
		*data = ctlr->phyoui[1];
		break;

	case MII_CTRL_REG:         // control
	case MII_AN_ADS_REG:       // autonegotiation advertisement
	case MII_AN_PRTN_REG:      // autonegotiation partner
	case MII_AN_EXP_REG:       // autonegotiation expansion
	case MII_AN_NEXT_REG:      // autonegotation next page transmit
	case MII_AN_PRTN_NEXT_REG: // link partner received next
	case MII_MASSLA_CTRL_REG:  // master-slave control
	case MII_MASSLA_STAT_REG:  // master-slave status
	case MII_EXT_STAT_REG:     // extended status
		*data = 0;
		break;

	default:
		printf("%s(phy=%d, reg=%#x): unknown reg\n", __func__, phyaddr, regaddr);
		break;
	}
	return OK;
}

LOCAL STATUS
dendphywrite(VXB_DEV_ID parent, UINT8 phyaddr, UINT8 regaddr, UINT16 data)
{
	printf("%s(phy=%d, reg=%d, data=%#x)\n", __func__, phyaddr, regaddr, data);
	return OK;
}

LOCAL STATUS
dendlinkupdate(VXB_DEV_ID parent)
{
	printf("%s(parent=%p)\n", __func__, parent);
	return OK;
}

LOCAL STATUS
dendlpimodeget(VXB_DEV_ID dev, UINT16 *mode)
{
	return OK;
}

LOCAL STATUS
dendphyattach(VXB_DEV_ID dev, VXB_DEV_ID miidev)
{
	Ctlr *ctlr;

	printf("%s(dev=%p, miidev=%p)\n", __func__, dev, miidev);
	ctlr = vxbDevSoftcGet(dev);
	ctlr->miidev = miidev;
	return OK;
}

LOCAL STATUS
dmiiattach(VXB_DEV_ID dev)
{
	MII_DRV_CTRL *miictlr;
	END_MEDIALIST *medialist;
	VXB_DEV_ID parent;
	PHY_HARDWARE *phydev;
	UINT16 miistat;

	printf("%s(dev=%p)\n", __func__, dev);

	miictlr = NULL;
	medialist = NULL;
	parent = vxbDevParent(dev);
	phydev = vxbDevIvarsGet(dev);
	if (!phydev)
		goto error;

	miictlr = vxbMemAlloc(sizeof(*miictlr));
	medialist = vxbMemAlloc(sizeof(*medialist));
	if (!miictlr || !medialist)
		goto error;

	miictlr->miiSelf = dev;
	miictlr->miiPhyAddr = phydev->phyAddr;
	miictlr->miiMediaList = medialist;
	miictlr->pInst = dev;
	miictlr->pEndInst = parent;
	miictlr->miiPhyList[0] = dev;
	miictlr->miiInitialized = TRUE;
	vxbDevSoftcSet(dev, miictlr);

	miiBusRead(dev, miictlr->miiPhyAddr, MII_STAT_REG, &miistat);
	if (miistat & MII_SR_EXT_STS) {
		miiBusMediaAdd(dev, IFM_ETHER | IFM_1000_T);
		miiBusMediaAdd(dev, IFM_ETHER | IFM_1000_T | IFM_FDX);
	}

	if (miistat & MII_SR_TX_HALF_DPX)
		miiBusMediaAdd(dev, IFM_ETHER | IFM_100_TX);
	if (miistat & MII_SR_TX_FULL_DPX)
		miiBusMediaAdd(dev, IFM_ETHER | IFM_100_TX | IFM_FDX);
	if (miistat & MII_SR_10T_HALF_DPX)
		miiBusMediaAdd(dev, IFM_ETHER | IFM_10_T);
	if (miistat & MII_SR_10T_FULL_DPX)
		miiBusMediaAdd(dev, IFM_ETHER | IFM_10_T | IFM_FDX);
	if (miistat & MII_SR_AUTO_SEL)
		miiBusMediaAdd(dev, IFM_ETHER | IFM_AUTO);

	miiBusMediaDefaultSet(dev, IFM_ETHER | IFM_AUTO);

	// add it to the MII device list
	// MII monitor bus uses this list to poll all the devices
	miiDevListAdd(dev);

	return OK;

error:
	vxbMemFree(miictlr);
	vxbMemFree(medialist);
	return ERROR;
}

LOCAL STATUS
dmiiprobe(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	PHY_HARDWARE *phydev;

	printf("%s(dev=%p)\n", __func__, dev);

	ctlr = vxbDevSoftcGet(vxbDevParent(dev));
	phydev = vxbDevIvarsGet(dev);
	if (!phydev)
		return ERROR;

	miiBusRead(dev, phydev->phyAddr, MII_PHY_ID1_REG, &phydev->phyId1);
	miiBusRead(dev, phydev->phyAddr, MII_PHY_ID2_REG, &phydev->phyId2);
	if (MII_OUI(phydev->phyId1, phydev->phyId2) != MII_OUI(ctlr->phyoui[0], ctlr->phyoui[1]))
		return ERROR;

	return OK;
}

LOCAL STATUS
dmiimodeset(VXB_DEV_ID dev, UINT32 mode)
{
	printf("%s(dev=%p, mode=%#x)\n", __func__, dev, mode);
	return OK;
}

LOCAL STATUS
dmiimodeget(VXB_DEV_ID dev, UINT32 *mode, UINT32 *status)
{
	Ctlr *ctlr;
	UINT16 miistat;

	printf("%s(dev=%p, mode=%p, status=%p)\n", __func__, dev, mode, status);

	ctlr = vxbDevSoftcGet(dev);
	*mode = IFM_ETHER;
	*status = IFM_AVALID;

	// read to unlatch link status bit
	miiBusRead(dev, ctlr->phyaddr, MII_STAT_REG, &miistat);
	// read again to know its value
	miiBusRead(dev, ctlr->phyaddr, MII_STAT_REG, &miistat);

	if (!(miistat & MII_SR_LINK_STATUS) || (miistat == 0xFFFF)) {
		*mode |= IFM_NONE;
		return OK;
	}

	*status |= IFM_ACTIVE;
	return OK;
}

LOCAL VXB_DRV_METHOD denddev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dendprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dendattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), denddetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dendshutdown },

	// resource allocation
	{ VXB_DEVMETHOD_CALL(vxbFdtDevGet), (FUNCPTR)denddevinfo },
	{ VXB_DEVMETHOD_CALL(vxbResourceAlloc), (FUNCPTR)dendresalloc },
	{ VXB_DEVMETHOD_CALL(vxbResourceFree), (FUNCPTR)dendresfree },
	{ VXB_DEVMETHOD_CALL(vxbResourceListGet), (FUNCPTR)dendreslistget },

	// usually this is a separate port driver
	// allows reading/writing to mii registers
	{ VXB_DEVMETHOD_CALL(miiRead), (FUNCPTR)dendphyread },
	{ VXB_DEVMETHOD_CALL(miiWrite), (FUNCPTR)dendphywrite },
	{ VXB_DEVMETHOD_CALL(miiMediaUpdate), (FUNCPTR)dendlinkupdate },
	{ VXB_DEVMETHOD_CALL(miiMediaAttach), (FUNCPTR)dendphyattach },
	{ VXB_DEVMETHOD_CALL(miiLpiModeGet), (FUNCPTR)dendlpimodeget },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD dmiidev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dmiiprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dmiiattach },
	{ VXB_DEVMETHOD_CALL(miiModeGet), dmiimodeget },
	{ VXB_DEVMETHOD_CALL(miiModeSet), dmiimodeset },
	VXB_DEVMETHOD_END
};

VXB_DRV denddrv = {
	{ NULL },
	"dummy-end",
	"Dummy END driver",
	VXB_BUSID_FDT,
	0,
	0,
	denddev,
	NULL,
};

VXB_DRV dmiidrv = {
	{ NULL },
	"dummy-mii",
	"Dummy MII driver",
	VXB_BUSID_MII,
	0,
	0,
	dmiidev,
	NULL,
};

VXB_DRV_DEF(denddrv)
VXB_DRV_DEF(dmiidrv)

STATUS
dendpoolshow(int unit)
{
	Ctlr *ctlr;

	ctlr = (void *)endFindByName("dend", unit);
	if (!ctlr)
		return ERROR;
	netPoolShow(ctlr->endobj.pNetPool);
	return OK;
}

STATUS
denddrvadd(void)
{
	STATUS status;

	status = 0;
	status |= vxbDrvAdd(&dmiidrv);
	status |= vxbDrvAdd(&denddrv);
	status |= ipcom_drv_eth_init("dend", 0, NULL);
	return status;
}

void
dendlink(void)
{
}
