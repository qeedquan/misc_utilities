#include "vxWorks.h"
#include "pingLib.h"

#undef main

int
main(int argc, char *argv[])
{
	pingLibInit();
	return vxmain(argc, argv);
}
