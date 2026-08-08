#include "vxWorks.h"
#include "usrFsLib.h"

STATUS
xcopy(const char *src, const char *dest)
{
	return OK;
}

void
pwd(void)
{
	char buf[PATH_MAX];

	getcwd(buf, sizeof(buf));
	printf("%s\n", buf);
}
