#include <vxWorks.h>
#include <stdio.h>
#include <hwif/vxBus.h>
#include <vxbMiiLib.h>
#include <muxTkLib.h>

STATUS
miidump(char *devname, int unit, int phyaddr)
{
	static const struct miireg {
		int off;
		const char *name;
	} regs[] = {
		{ MII_CTRL_REG, "CONTROL" },
		{ MII_STAT_REG, "STATUS" },
		{ MII_PHY_ID1_REG, "PHYID1" },
		{ MII_PHY_ID2_REG, "PHYID2" },
		{ MII_AN_ADS_REG, "AUTO-NEGOTIATION ADVERTISEMENT" },
		{ MII_AN_PRTN_REG, "AUTO-NEGOTIATION PARTNER ABILITY" },
		{ MII_AN_EXP_REG, "AUTO-NEGOTIATION NEXT PAGE" },
		{ MII_AN_NEXT_REG, "AUTO-NEGOTIATION LINK PARTNER" },
		{ MII_MASSLA_CTRL_REG, "MASTER-SLAVE CONTROL" },
		{ MII_MASSLA_STAT_REG, "MASTER-SLAVE STATUS" },
		{ MII_EXT_STAT_REG, "EXTENDED STATUS" },
	};

	VXB_DEV_ID dev;
	UINT16 val;
	size_t i;

	dev = vxbDevAcquireByName(devname, unit);
	if (!dev) {
		printf("Failed to get device\n");
		return ERROR;
	}

	for (i = 0; i < NELEMENTS(regs); i++) {
		if (MII_READ(dev, phyaddr, regs[i].off, &val) != OK)
			printf("%-32s: Failed to read\n", regs[i].name);
		else
			printf("%-32s: %#x\n", regs[i].name, val);
	}

	vxbDevRelease(dev);
	return OK;
}

STATUS
etherinfo(void *cookie, void *arg)
{
	END_IFCOUNTERS *ifcnt;
	END_CAPABILITIES hwcaps;
	volatile END_MEDIA media;
	STATUS status;
	volatile int flags;
	int mtu;
	char mac[6];

	status = muxIoctl(cookie, EIOCGIFMEDIA, (void *)&media);
	if (status == OK)
		printf("Media: Status: %#x Active: %#x\n", media.endMediaStatus, media.endMediaActive);

	status = muxIoctl(cookie, EIOCGFLAGS, (void *)&flags);
	if (status == OK)
		printf("Flags: %#x\n", flags);

	status = muxIoctl(cookie, EIOCGADDR, (void *)mac);
	if (status == OK)
		printf("MAC: %02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	status = muxIoctl(cookie, EIOCGIFMTU, (void *)&mtu);
	if (status == OK)
		printf("MTU: %d\n", mtu);

	status = muxIoctl(cookie, EIOCGIFCAP, (void *)&hwcaps);
	if (status == OK)
		printf("HWCAPS: Available: %#x Enabled: %#x\n", hwcaps.cap_available, hwcaps.cap_enabled);

	status = muxIoctl(cookie, EIOCGPOLLSTATS, (void *)&ifcnt);
	if (status == OK) {
		printf("Poll Stats\n");
		printf("\tifInErrors         %llu\n", ifcnt->ifInErrors);
		printf("\tifInDiscards       %llu\n", ifcnt->ifInDiscards);
		printf("\tifInUnknownProtos  %llu\n", ifcnt->ifInUnknownProtos);
		printf("\tifInOctets         %llu\n", ifcnt->ifInOctets);
		printf("\tifInUcastPkts      %llu\n", ifcnt->ifInUcastPkts);
		printf("\tifInMulticastPkts  %llu\n", ifcnt->ifInMulticastPkts);
		printf("\tifInBroadcastPkts  %llu\n", ifcnt->ifInBroadcastPkts);
		printf("\tifOutErrors        %llu\n", ifcnt->ifOutErrors);
		printf("\tifOutDiscards      %llu\n", ifcnt->ifOutDiscards);
		printf("\tifOutOctets        %llu\n", ifcnt->ifOutOctets);
		printf("\tifOutUcastPkts     %llu\n", ifcnt->ifOutUcastPkts);
		printf("\tifOutMulticastPkts %llu\n", ifcnt->ifOutMulticastPkts);
		printf("\tifOutBroadcastPkts %llu\n", ifcnt->ifOutBroadcastPkts);
		printf("\n");
	}

	return OK;

	(void)arg;
}

STATUS
etherpoll(void *cookie, void *arg)
{
	STATUS status;
	int on;

	on = (int)arg;
	if (on)
		status = muxIoctl(cookie, EIOCPOLLSTART, NULL);
	else
		status = muxIoctl(cookie, EIOCPOLLSTOP, NULL);

	if (status == OK)
		printf("Set poll mode to: %d\n", on);
	else
		printf("Failed to set poll mode\n");

	return status;
}

STATUS
etherioctl(char *name, int unit, void *arg, STATUS (*func)(void *, void *))
{
	void *cookie;

	cookie = muxTkCookieGet(name, unit);
	if (!cookie) {
		printf("Failed to get cookie\n");
		return ERROR;
	}
	return func(cookie, arg);
}
