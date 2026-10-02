#include <stdio.h>
#include <err.h>
#include <sys/utsname.h>

int
main(void)
{
	struct utsname u;

	if (uname(&u) < 0)
		err(1, "uname");

	printf("sysname:   %s\n", u.sysname);
	printf("nodename:  %s\n", u.nodename);
	printf("release:   %s\n", u.release);
	printf("version:   %s\n", u.version);
	printf("machine:   %s\n", u.machine);

	return 0;
}
