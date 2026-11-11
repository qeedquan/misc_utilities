// ported from 9front
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

unsigned char odata[16];
unsigned char data[16];
int ndata;
unsigned long addr;
int repeats;
int le; /* little endian */
int flush;
int abase = 2;

int xd(const char *, int);
void xprint(const char *, ...);
void initarg(void);
void swizz(void);

enum {
	Narg = 10
};

typedef struct Arg Arg;
typedef void fmtfn(const char *);

struct Arg {
	int ascii;        /* 0==none, 1==ascii */
	int loglen;       /* 0==1, 1==2, 2==4, 3==8 */
	int base;         /* 0==8, 1==10, 2==16 */
	fmtfn *fn;        /* function to call with data */
	const char *afmt; /* format to use to print address */
	const char *fmt;  /* format to use to print data */
} arg[Narg];
int narg;

fmtfn fmt0, fmt1, fmt2, fmt3, fmtc;
fmtfn *fmt[4] = {
    fmt0,
    fmt1,
    fmt2,
    fmt3,
};

const char *dfmt[4][3] = {
    {" %.3o", " %.3d", " %.2x"},
    {" %.6o", " %.5d", " %.4x"},
    {" %.11lo", " %.10ld", " %.8lx"},
    {" %.22llo", " %.20lld", " %.16llx"},
};

const char *cfmt[3][3] = {
    {"   %c", "   %c", "  %c"},
    {" %.3s", " %.3s", " %.2s"},
    {" %.3o", " %.3d", " %.2x"},
};

const char *afmt[2][3] = {
    {"%.7lo ", "%.7ld ", "%.7lx "},
    {"%7lo ", "%7ld ", "%7lx "},
};

FILE *bin;
FILE *bout;

int
main(int argc, char *argv[])
{
	int i, err;
	Arg *ap;

	bin = stdin;
	bout = stdout;
	err = 0;
	ap = 0;
	while (argc > 1 && argv[1][0] == '-' && argv[1][1]) {
		--argc;
		argv++;
		argv[0]++;
		if (argv[0][0] == 'r') {
			repeats = 1;
			if (argv[0][1])
				goto Usage;
			continue;
		}
		if (argv[0][0] == 's') {
			le = 1;
			if (argv[0][1])
				goto Usage;
			continue;
		}
		if (argv[0][0] == 'u') {
			flush = 1;
			if (argv[0][1])
				goto Usage;
			continue;
		}
		if (argv[0][0] == 'a') {
			argv[0]++;
			switch (argv[0][0]) {
			case 'o':
				abase = 0;
				break;
			case 'd':
				abase = 1;
				break;
			case 'x':
				abase = 2;
				break;
			default:
				goto Usage;
			}
			if (argv[0][1])
				goto Usage;
			continue;
		}
		ap = &arg[narg];
		initarg();
		while (argv[0][0]) {
			switch (argv[0][0]) {
			case 'c':
				ap->ascii = 1;
				ap->loglen = 0;
				if (argv[0][1] || argv[0][-1] != '-')
					goto Usage;
				break;
			case 'o':
				ap->base = 0;
				break;
			case 'd':
				ap->base = 1;
				break;
			case 'x':
				ap->base = 2;
				break;
			case 'b':
			case '1':
				ap->loglen = 0;
				break;
			case 'w':
			case '2':
				ap->loglen = 1;
				break;
			case 'l':
			case '4':
				ap->loglen = 2;
				break;
			case 'v':
			case '8':
				ap->loglen = 3;
				break;
			default:
			Usage:
				fprintf(stderr, "usage: xd [-u] [-r] [-s] [-a{odx}] [-c|{b1w2l4v8}{odx}] ... file ...\n");
				exit(2);
			}
			argv[0]++;
		}
		if (ap->ascii)
			ap->fn = fmtc;
		else
			ap->fn = fmt[ap->loglen];
		ap->fmt = dfmt[ap->loglen][ap->base];
		ap->afmt = afmt[ap > arg][abase];
	}
	if (narg == 0)
		initarg();
	if (argc == 1)
		err = xd(0, 0);
	else if (argc == 2)
		err = xd(argv[1], 0);
	else
		for (i = 1; i < argc; i++)
			err |= xd(argv[i], 1);
	return err;
}

void
initarg(void)
{
	Arg *ap;

	ap = &arg[narg++];
	if (narg >= Narg) {
		fprintf(stderr, "xd: too many formats (max %d)\n", Narg);
		exit(2);
	}
	ap->ascii = 0;
	ap->loglen = 2;
	ap->base = 2;
	ap->fn = fmt2;
	ap->fmt = dfmt[ap->loglen][ap->base];
	ap->afmt = afmt[narg > 1][abase];
}

int
xd(const char *name, int title)
{
	int i, star;
	Arg *ap;
	FILE *bp;

	if (name) {
		bp = fopen(name, "rb");
		if (bp == NULL) {
			fprintf(stderr, "xd: can't open %s\n", name);
			return 1;
		}
	} else {
		bp = stdin;
	}
	if (flush)
		fflush(bp);
	if (title)
		xprint("%s\n", name);
	addr = 0;
	star = 0;
	while ((ndata = fread(data, 1, 16, bp)) >= 0) {
		if (ndata < 16)
			for (i = ndata; i < 16; i++)
				data[i] = 0;
		if (ndata == 16 && repeats) {
			if (addr > 0 && data[0] == odata[0]) {
				for (i = 1; i < 16; i++)
					if (data[i] != odata[i])
						break;
				if (i == 16) {
					addr += 16;
					if (star == 0) {
						star++;
						xprint("*\n", 0);
					}
					continue;
				}
			}
			for (i = 0; i < 16; i++)
				odata[i] = data[i];
			star = 0;
		}
		for (ap = arg; ap < &arg[narg]; ap++) {
			xprint(ap->afmt, addr);
			(*ap->fn)(ap->fmt);
			xprint("\n", 0);
		}
		addr += ndata;
		if (ndata < 16) {
			xprint(afmt[0][abase], addr);
			xprint("\n", 0);
			break;
		}
	}
	fclose(bp);
	return 0;
}

void
fmt0(const char *f)
{
	int i;
	for (i = 0; i < ndata; i++)
		xprint(f, data[i]);
}

void
fmt1(const char *f)
{
	int i;
	for (i = 0; i < ndata; i += sizeof(uint16_t))
		xprint(f, le ? (data[i + 1] << 8) | data[i]
		             : (data[i] << 8) | data[i + 1]);
}

void
fmt2(const char *f)
{
	int i;
	for (i = 0; i < ndata; i += sizeof(uint32_t))
		xprint(f, le ? (data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i]
		             : (data[i] << 24) | (data[i + 1] << 16) | (data[i + 2] << 8) | data[i + 3]);
}

void
fmt3(const char *f)
{
	int i;
	uint64_t v;
	for (i = 0; i < ndata; i += sizeof(uint64_t)) {
		if (le) {
			v = (data[i + 3 + 4] << 24) | (data[i + 2 + 4] << 16) | (data[i + 1 + 4] << 8) | data[i + 4];
			v <<= 32;
			v |= (data[i + 3] << 24) | (data[i + 2] << 16) | (data[i + 1] << 8) | data[i];
		} else {
			v = (data[i] << 24) | (data[i + 1] << 16) | (data[i + 2] << 8) | data[i + 3];
			v <<= 32;
			v |= (data[i + 4] << 24) | (data[i + 1 + 4] << 16) | (data[i + 2 + 4] << 8) | data[i + 3 + 4];
		}
		if (fprintf(bout, f, v) < 0) {
			fprintf(stderr, "xd: i/o error\n");
			exit(1);
		}
	}
}

void
fmtc(const char *f)
{
	int i;

	for (i = 0; i < ndata; i++) {
		switch (data[i]) {
		case '\t':
			xprint(cfmt[1][2], "\\t");
			break;
		case '\r':
			xprint(cfmt[1][2], "\\r");
			break;
		case '\n':
			xprint(cfmt[1][2], "\\n");
			break;
		case '\b':
			xprint(cfmt[1][2], "\\b");
			break;
		default:
			if (data[i] >= 0x7F || ' ' > data[i])
				xprint(cfmt[2][2], data[i]);
			else
				xprint(cfmt[0][2], data[i]);
			break;
		}
	}

	(void)f;
}

void
xprint(const char *fmt, ...)
{
	va_list arglist;

	va_start(arglist, fmt);
	if (vfprintf(bout, fmt, arglist) < 0) {
		fprintf(stderr, "xd: i/o error\n");
		exit(1);
	}
	va_end(arglist);
}
