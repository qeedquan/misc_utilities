#include <u.h>
#include <libc.h>

char *port = "3000";

int dfd;

void
usage(void)
{
	fprint(2, "usage: [-p] [file] ...\n");
	fprint(2, "\t-p: port (default = %s)\n", port);
	exits("usage");
}

void
cat(int fd)
{
	char buf[8192];
	int  n;

	for (;;) {
		n = read(fd, buf, sizeof(buf));
		if (n < 0)
			sysfatal("read: %r");
		if (n == 0)
			break;
		if (write(dfd, buf, n) <= 0)
			sysfatal("write: %r");
	}
}

void
readfile(char *name)
{
	int fd;

	fd = open(name, OREAD);
	if (fd < 0)
		sysfatal("open: %r");
	cat(fd);
	close(fd);
}

void
main(int argc, char *argv[])
{
	char buf[1024], adir[40], ldir[40];
	int  acfd, lcfd, i;

	ARGBEGIN
	{
	case 'h':
		usage();
	case 'p':
		port = EARGF(usage());
		break;
	}
	ARGEND;

	snprint(buf, sizeof(buf), "tcp!*!%s", port);
	acfd = announce(buf, adir);
	if (acfd < 0)
		sysfatal("announce: %r");

	lcfd = listen(adir, ldir);
	if (lcfd < 0)
		sysfatal("listen: %r");

	dfd = accept(lcfd, ldir);
	if (dfd < 0)
		sysfatal("accept: %r");

	if (argc < 1)
		cat(0);
	else {
		for (i = 0; i < argc; i++)
			readfile(argv[i]);
	}

	exits(0);
}
