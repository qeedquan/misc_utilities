#include <string.h>
#include <sys/utsname.h>

int
vxuname(struct utsname *buf)
{
	memset(buf, 0, sizeof(*buf));
	return 0;
}
