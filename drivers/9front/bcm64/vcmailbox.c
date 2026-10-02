#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: %s <word> ...\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	u32int buf[256];
	char *rptr;
	int i, fd;
	long len;

	ARGBEGIN
	{
	}
	ARGEND;

	if (argc < 1 || argc >= nelem(buf))
		usage();

	fd = open("/dev/vcio", ORDWR);
	if (fd < 0)
		sysfatal("%s: %r", argv0);

	for (i = 0; i < argc; i++)
		buf[i] = strtoul(argv[i], &rptr, 0);

	len = sizeof(*buf) * argc;
	if (pwrite(fd, buf, len, 0) != len)
		sysfatal("%s: write failed", argv0);

	if ((len = pread(fd, buf, sizeof(buf), 0)) < 0)
		sysfatal("%s: read failed", argv0);

	len >>= 2;
	for (i = 0; i < len; i++) {
		print("%08lx", buf[i]);
		if (i + 1 < len)
			print(" ");
	}
	print("\n");
	close(fd);

	exits(0);
}
