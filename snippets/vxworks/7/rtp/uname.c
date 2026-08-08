#include <stdio.h>
#include <err.h>
#include <sys/utsname.h>

int
main(int argc, char *argv[])
{
	struct utsname u;

	if (uname(&u) < 0)
		err(1, "uname");

	printf("sysname:              %s\n", u.sysname);
	printf("nodename:             %s\n", u.nodename);
	printf("release:              %s\n", u.release);
	printf("version:              %s\n", u.version);
	printf("machine:              %s\n", u.machine);
	printf("endian:               %s\n", u.endian);
	printf("kernel version:       %s\n", u.kernelversion);
	printf("release version:      %s\n", u.releaseversion);
	printf("processor:            %s\n", u.processor);
	printf("bsp revision:         %s\n", u.bsprevision);
	printf("build date:           %s\n", u.builddate);

	return 0;
}
