/*

netDrv filesystem is implemented by having function pointers that gets called on cd/ls
this shows how to intercept it to print what directories we are looking at

*/

#include <vxWorks.h>
#include <stdio.h>

extern FUNCPTR _func_netChDirByName;
extern FUNCPTR _func_netLsByName;

FUNCPTR _func_netChDirByName_saved;
FUNCPTR _func_netLsByName_saved;

STATUS
netdrvchdir(const char *dirname)
{
	printf("%s(dirname=%s)\n", __func__, dirname);
	if (_func_netChDirByName_saved)
		return _func_netChDirByName_saved(dirname);
	return ERROR;
}

STATUS
netdrvls(char *dirname, int fd)
{
	printf("%s(dirname=%s, fd=%d)\n", __func__, dirname, fd);
	if (_func_netLsByName_saved)
		return _func_netLsByName_saved(dirname, fd);
	return ERROR;
}

void
netdrvinit(void)
{
	_func_netChDirByName_saved = _func_netChDirByName;
	_func_netLsByName_saved = _func_netLsByName;

	_func_netChDirByName = netdrvchdir;
	_func_netLsByName = netdrvls;
}
