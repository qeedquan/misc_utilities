#include "vxWorks.h"
#include "inetLib.h"

void
inet_ntoa_b(struct in_addr inetAddress, char *pString)
{
	char *p;

	p = (char *)&inetAddress;
	if (pString)
		sprintf(pString, "%d.%d.%d.%d", p[0] & 0xff, p[1] & 0xff, p[2] & 0xff, p[3] & 0xff);
}