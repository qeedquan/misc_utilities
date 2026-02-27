#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main(void)
{
	char *str;

	str = getpass("> ");
	printf("< %s\n", str);
	free(str);

	return 0;
}
