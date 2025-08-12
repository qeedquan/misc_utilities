#include <vxWorks.h>
#include <stdio.h>
#include <muxTkLib.h>

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

	(void) arg;
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
