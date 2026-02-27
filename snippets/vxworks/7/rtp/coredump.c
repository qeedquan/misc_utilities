// Compile the VIP with RTP coredump support
// When called to abort() it will generate a corefile that can be converted into a linux corefile format using the command
// vx_coredump_agent -vxCore coredumpRTP.vxcore -vxElfGen linuxcorefile -vxGdbTdesc Installation/workbench-4/release/x86_64-linux2/share/vx_gdb_extensions/gdb_target_amd64.xml
// The generated 'linuxcorefile' can then be used in GDB with 'core-file linuxcorefile'

#include <stdlib.h>

int
main(void)
{
	int a, b, c;

	a = 20;
	b = 30;
	c = 40;
	abort();
	return a + b + c;
}
