#include <vxWorks.h>
#include <stdio.h>
#include <ipaddrEventLib.h>
#include <netinet/in.h>
#include <netinet6/in6.h>

// called when IP address is added/removed/in conflict/passes duplicate address detection
// use ifconfig to bring device up/down to test
void
iphook(IPADDR_EVENT event, unsigned int ifindex, void *address, void *mac)
{
	kprintf("%s(event=%d, ifindex=%u, address=%p, mac=%p)\n", __func__, __LINE__, event, ifindex, address, mac);
}

void
iphooktest(void)
{
	ipAddressEventHookAdd(iphook);
}
