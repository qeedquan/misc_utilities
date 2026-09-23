#define _GNU_SOURCE
#include <stdio.h>

char *
scanstr(char *buf, size_t len)
{
	char fmt[256];

	if (len == 0)
		return buf;
	snprintf(fmt, sizeof(fmt), "%%%zus", len - 1);
	scanf(fmt, buf);
	return buf;
}

int
main(void)
{
	char buf[4];

	printf("% d\n", -1);
	printf("% d\n", 1);
	printf("%2$d %2$#x; %1$d %1$#x\n", 10, 11);
	printf("%+1d %-1d\n", 20, -20);
	printf("%s\n", scanstr(buf, sizeof(buf)));
	return 0;
}
