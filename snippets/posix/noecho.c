#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <termios.h>

void
setecho(int fd, int enb)
{
	struct termios t;
	tcgetattr(fd, &t);
	if (enb)
		t.c_lflag |= ECHO;
	else
		t.c_lflag &= ~ECHO;
	tcsetattr(fd, TCSANOW, &t);
}

int
main(void)
{
	char line[128];
	int fd;

	memset(line, 0, sizeof(line));
	fd = fileno(stdin);

	printf("enter input: ");
	setecho(fd, 0);
	fgets(line, sizeof(line), stdin);
	setecho(fd, 1);
	printf("\nyou entered: %s\n", line);

	return 0;
}
