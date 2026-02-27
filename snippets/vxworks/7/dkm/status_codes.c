#include <stdio.h>
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
#include <scMemVal.h>

#define F(x)                        \
	{                           \
		printf("%s\n", #x); \
		x();                \
		printf("\n");       \
	}

#define P(x) printf("%s: %#x\n", #x, x);

void
sclib(void)
{
	P(SC_PROT_READ);
	P(SC_PROT_WRITE);
	P(SC_PROT_ATOMIC_RMW);
}

void
ethlib(void)
{
	P(EIOCSADDR);
	P(EIOCGADDR);
	P(EIOCSFLAGS);
	P(EIOCGFLAGS);
	P(EIOCPOLLSTART);
	P(EIOCPOLLSTOP);
	P(EIOCGMIB2233);
	P(EIOCGMIB2);
	P(EIOCGPOLLCONF);
	P(EIOCGPOLLSTATS);
	P(EIOCGMEDIALIST);
	P(EIOCGIFMEDIA);
	P(EIOCSIFMEDIA);
	P(EIOCGIFCAP);
	P(EIOCSIFCAP);
	P(EIOCGIFMTU);
	P(EIOCSIFMTU);
	P(EIOCGRCVJOBQ);
	P(EIOCSRCVJOBQ);
	P(EIOCSVIDFILTER);
	P(EIOCCVIDFILTER);
}

void
print_status_codes(void)
{
	F(sclib);
	F(ethlib);
}
