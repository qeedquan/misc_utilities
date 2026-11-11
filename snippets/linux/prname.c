#include <stdio.h>
#include <string.h>
#include <err.h>
#include <sys/prctl.h>

void
xprctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5)
{
	if (prctl(option, arg2, arg3, arg4, arg5) < 0)
		err(1, "prctl");
}

int
main(void)
{
	char name[80];

	xprctl(PR_GET_NAME, (unsigned long)name, 0, 0, 0);
	printf("%s\n", name);

	strcpy(name, "test");
	xprctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
	printf("%s\n", name);

	// the process tree should have this process as named 'test' now
	for (;;)
		;

	return 0;
}
