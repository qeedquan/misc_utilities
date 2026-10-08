#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pty.h>
#include <err.h>

int
main(int argc, char *argv[])
{
	char buf[8192], *sh;
	int m, s;
	ssize_t i, nr;

	if (argc >= 2)
		sh = argv[1];
	else
		sh = "/bin/bash";

	if (openpty(&m, &s, NULL, NULL, NULL) < 0)
		err(1, "openpty");

	switch (fork()) {
	case -1:
		err(1, "fork");
	case 0:
		setsid();
		dup2(s, 0);
		dup2(s, 1);
		dup2(s, 2);
		if (ioctl(s, TIOCSCTTY, NULL) < 0)
			err(1, "ioctl");
		close(m);
		close(s);
		execvp(sh, (char *[]){sh, NULL});
		_exit(1);
		break;
	default:
		close(s);
		break;
	}

	nr = read(m, buf, sizeof(buf));
	if (nr < 0)
		err(1, "read");
	for (i = 0; i < nr; i++)
		printf("%c", buf[i]);
	printf("\n");

	return 0;
}
