// ported from 9front
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF 65536

typedef unsigned char uchar;
typedef long long vlong;

int sflag = 0;
int lflag = 0;
int Lflag = 0;

static void usage(void);

int
isascii(int c)
{
	return c < 127;
}

void
exits(const char *msg)
{
	if (!msg) {
		fprintf(stdout, "same file\n");
		exit(0);
	} else if (!strcmp(msg, "usage")) {
		exit(2);
	} else {
		exit(1);
	}
}

char **
seekoff(int fd, char *name, char **argv)
{
	vlong o;

	if (*argv) {
		if (!isascii(**argv) || !isdigit(**argv))
			usage();
		o = strtoll(*argv++, 0, 0);
		if (lseek(fd, o, SEEK_SET) < 0) {
			if (!sflag)
				fprintf(stderr, "cmp: %s: seek by %lld: %s\n",
				    name, o, strerror(errno));
			exits("seek");
		}
	}
	return argv;
}

int
main(int argc, char *argv[])
{
	int i;
	uchar *p, *q;
	uchar buf1[BUF], buf2[BUF];
	int f1, f2;
	vlong nc = 1, l = 1;
	ssize_t n;
	char *name1, *name2;
	uchar *b1s, *b1e, *b2s, *b2e;
	int c;

	while ((c = getopt(argc, argv, "s:l:L:")) != -1) {
		switch (c) {
		case 's':
			sflag = 1;
			break;
		case 'l':
			lflag = 1;
			break;
		case 'L':
			Lflag = 1;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 2 || argc > 4)
		usage();
	if ((f1 = open(name1 = *argv++, O_RDONLY)) == -1) {
		if (!sflag)
			perror(name1);
		exits("open");
	}
	if ((f2 = open(name2 = *argv++, O_RDONLY)) == -1) {
		if (!sflag)
			perror(name2);
		exits("open");
	}
	argv = seekoff(f1, name1, argv);
	argv = seekoff(f2, name2, argv);
	if (*argv)
		usage();

	b1s = b1e = buf1;
	b2s = b2e = buf2;
	for (;;) {
		if (b1s >= b1e) {
			if (b1s >= &buf1[BUF])
				b1s = buf1;
			n = read(f1, b1s, &buf1[BUF] - b1s);
			b1e = b1s + n;
		}
		if (b2s >= b2e) {
			if (b2s >= &buf2[BUF])
				b2s = buf2;
			n = read(f2, b2s, &buf2[BUF] - b2s);
			b2e = b2s + n;
		}
		n = b2e - b2s;
		if (n > b1e - b1s)
			n = b1e - b1s;
		if (n <= 0)
			break;
		if (memcmp((void *)b1s, (void *)b2s, n) != 0) {
			if (sflag)
				exits("differ");
			for (p = b1s, q = b2s, i = 0; i < n; p++, q++, i++) {
				if (*p == '\n')
					l++;
				if (*p != *q) {
					if (!lflag) {
						printf("%s %s differ: char %lld",
						    name1, name2, nc + i);
						printf(Lflag ? " line %lld\n" : "\n", l);
						exits("differ");
					}
					printf("%6lld 0x%.2x 0x%.2x\n", nc + i, *p, *q);
				}
			}
		}
		if (Lflag)
			for (p = b1s; p < b1e;)
				if (*p++ == '\n')
					l++;
		nc += n;
		b1s += n;
		b2s += n;
	}
	if (b1e - b1s < 0 || b2e - b2s < 0) {
		if (!sflag) {
			if (b1e - b1s < 0)
				printf("error on %s after %lld bytes\n",
				    name1, nc - 1);
			if (b2e - b2s < 0)
				printf("error on %s after %lld bytes\n",
				    name2, nc - 1);
		}
		exits("read error");
	}
	if (b1e - b1s == b2e - b2s)
		exits((char *)0);
	if (!sflag)
		printf("EOF on %s after %lld bytes\n",
		    (b1e - b1s > b2e - b2s) ? name2 : name1, nc - 1);
	exits("EOF");
	return 0;
}

static void
usage(void)
{
	fprintf(stderr, "usage: cmp [-lLs] file1 file2 [offset1 [offset2] ]\n");
	exits("usage");
}
