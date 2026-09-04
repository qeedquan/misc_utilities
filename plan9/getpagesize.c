#include <u.h>
#include <libc.h>
#include <bio.h>

int
getpagesize(void)
{
	int pgsize, n;
	char *s, *f[2];
	Biobuf *bp;

	pgsize = 0x1000;
	bp = Bopen("/dev/swap", OREAD);
	if (bp == nil)
		return pgsize;

	while ((s = Brdline(bp, '\n'))) {
		if ((n = Blinelen(bp)) < 1)
			continue;
		s[n - 1] = '\0';
		if (tokenize(s, f, nelem(f)) != 2)
			continue;
		if (strcmp(f[1], "pagesize") == 0) {
			pgsize = strtoul(f[0], 0, 0);
			break;
		}
	}
	Bterm(bp);

	return pgsize;
}

void
main(void)
{
	print("%d\n", getpagesize());
	exits(nil);
}
