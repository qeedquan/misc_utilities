// based on http://free-electrons.com/pub/mirror/devmem2.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <sys/mman.h>

static void
usage(void)
{
	fprintf(stderr, "usage: address [type [ data ] ]\n");
	fprintf(stderr, "  address : memory address\n");
	fprintf(stderr, "  type    : access operation : [b]yte [h]alfword [w]ord [d]word\n");
	exit(2);
}

static unsigned long
readmem(void *addr, int access)
{
	switch (access) {
	case 'b':
		return *((unsigned char *)addr);
	case 'h':
		return *((unsigned short *)addr);
	case 'w':
		return *((unsigned int *)addr);
	case 'd':
		return *((unsigned long *)addr);
	}
	errx(1, "unknown data type '%c'\n", access);
}

static unsigned long
writemem(void *addr, unsigned long val, int access)
{
	switch (access) {
	case 'b':
		*((unsigned char *)addr) = val;
		break;
	case 'h':
		*((unsigned short *)addr) = val;
		break;
	case 'w':
		*((unsigned int *)addr) = val;
		break;
	case 'd':
		*((unsigned long *)addr) = val;
		break;
	default:
		errx(1, "unknown data type '%c'\n", access);
	}
	return readmem(addr, access);
}

int
main(int argc, char *argv[])
{
	unsigned long val, rval;
	int fd, access, pagesz;
	off_t target;
	void *mem, *addr;

	if (argc < 2)
		usage();

	target = strtoul(argv[1], 0, 0);
	access = 'w';
	if (argc > 2)
		access = tolower(argv[2][0]);

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0)
		err(1, "open: /dev/mem");

	pagesz = getpagesize();
	mem = mmap(0, pagesz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~(pagesz - 1));
	if (mem == MAP_FAILED)
		err(1, "mmap");
	printf("memory mapped at address %p\n", mem);

	addr = mem + (target & (pagesz - 1));
	val = readmem(addr, access);
	printf("value at address %#zx (%p): %#lx\n", target, addr, val);

	if (argc > 3) {
		val = strtoul(argv[3], 0, 0);
		rval = writemem(addr, val, access);
		printf("written %#lx, readback %#lx\n", val, rval);
	}

	close(fd);
	return 0;
}
