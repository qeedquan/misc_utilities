#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: [-s size] <addr> [value]\n");
	exits("usage");
}

void
main(int argc, char *argv[])
{
	char *name;
	ulong addr, val;

	uchar valb;
	ushort vals;
	ulong vall;
	void *valp;

	int fd, size;

	name = nil;
	size = 1;
	ARGBEGIN
	{
	case 's':
		size = atoi(EARGF(usage()));
		break;

	default:
		usage();
		break;
	}
	ARGEND;

	if (argc < 1)
		usage();

	addr = strtoul(argv[0], nil, 0);
	val = (argc >= 2) ? strtoul(argv[1], nil, 0) : 0;
	valb = vals = vall = val;

	switch (size) {
	case 1:
		name = "/dev/iob";
		valp = &valb;
		break;
	case 2:
		name = "/dev/iow";
		valp = &vals;
		break;
	case 4:
		name = "/dev/iol";
		valp = &vall;
		break;
	default:
		sysfatal("invalid io size: %d", size);
		break;
	}

	fd = open(name, ORDWR);
	if (fd < 0)
		sysfatal("open: %r");

	if (argc >= 2) {
		if (pwrite(fd, valp, size, addr) != size)
			sysfatal("write: %r");
	} else {
		if (pread(fd, valp, size, addr) != size)
			sysfatal("read: %r");

		switch (size) {
		case 1:
			print("%x", valb);
			break;
		case 2:
			print("%x", vals);
			break;
		case 4:
			print("%lx", vall);
			break;
		}
	}

	exits(nil);
}
