#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <loadLib.h>
#include <symLib.h>
#include <sysSymTbl.h>

int
loadlib(const char *module, const char *symbol)
{
	SYMBOL_DESC sd;
	MODULE_ID md;
	int fd;
	int r;

	r = 0;
	fd = open(module, O_RDONLY);
	if (fd < 0)
		goto error;

	md = loadModule(fd, LOAD_GLOBAL_SYMBOLS);
	if (md == NULL)
		goto error;

	memset(&sd, 0, sizeof(sd));
	sd.mask = SYM_FIND_BY_NAME;
	sd.name = (char *)symbol;
	if (symFind(sysSymTbl, &sd) == OK)
		r = 1;

	if (0) {
	error:
		r = -1;
	}

	if (fd >= 0)
		close(fd);

	return r;
}
