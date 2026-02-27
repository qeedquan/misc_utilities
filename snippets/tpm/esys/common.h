#ifndef _COMMON_H_
#define _COMMON_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <err.h>
#include <getopt.h>
#include <tss2/tss2_esys.h>
#include <tss2/tss2_rc.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct Option Option;

struct Option {
	ESYS_TR hierarchy;
	TPM2B_AUTH password;
	TPMI_ALG_HASH hashalg;

	char extraopts[32];
	void (*handler)(Option *, int);
	void *userdata;
};

void
fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

void
check(const char *f, int r)
{
	if (r != TSS2_RC_SUCCESS)
		errx(1, "%s: %d: %s", f, r, Tss2_RC_Decode(r));
}

ESYS_TR
get_hierarchy(const char *s)
{
	if (!strcmp(s, "o") || !strcmp(s, "owner"))
		return ESYS_TR_RH_OWNER;
	if (!strcmp(s, "p") || !strcmp(s, "platform"))
		return ESYS_TR_RH_PLATFORM;
	if (!strcmp(s, "l") || !strcmp(s, "lockout"))
		return ESYS_TR_RH_LOCKOUT;
	return ESYS_TR_NONE;
}

void
parseopt(Option *o, int *argc, char ***argv)
{
	char optstr[64];
	int c;

	snprintf(optstr, sizeof(optstr), "c:P:%s", o->extraopts);
	while ((c = getopt(*argc, *argv, optstr)) != -1) {
		switch (c) {
		case 'c':
			o->hierarchy = get_hierarchy(optarg);
			break;

		case 'P':
			o->password.size = strlen(optarg);
			if (o->password.size >= nelem(o->password.buffer))
				fatal("Specified authentication password too long, max size is %zu", nelem(o->password.buffer));
			memcpy(o->password.buffer, optarg, o->password.size);
			break;

		default:
			if (o->handler)
				o->handler(o, c);
			break;
		}
	}
	*argc -= optind;
	*argv += optind;
}

ESYS_CONTEXT *
newctx(Option *o)
{
	ESYS_CONTEXT *c;
	int r;

	r = Esys_Initialize(&c, NULL, NULL);
	check("Esys_Initialize", r);

	// if changeauth is used to set a password, every command on that hierarchy
	// needs to specify a password for using the command, otherwise the command will fail to execute
	if (o->password.size) {
		r = Esys_TR_SetAuth(c, o->hierarchy, &o->password);
		check("Esys_Tr_SetAuth", r);
	}

	return c;
}

void
hexdump(const void *b, size_t n)
{
	const uint8_t *p;
	size_t i;

	p = b;
	for (i = 0; i < n; i++) {
		printf("%02x ", p[i]);
		if ((i & 15) == 0)
			printf("\n");
	}
	printf("\n");
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;

	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;
	p = calloc(nmemb, size);
	if (!p)
		abort();
	return p;
}

void
genrand(void *buf, size_t len)
{
	uint8_t *p;
	size_t i;

	p = buf;
	for (i = 0; i < len; i++)
		p[i] = rand() & 0xff;
}

void *
read_file(const char *name, size_t *len)
{
	FILE *fp;
	void *p;
	long n;

	fp = fopen(name, "rb");
	if (!fp)
		goto error;

	fseek(fp, 0, SEEK_END);
	n = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (n < 0)
		goto error;

	p = xcalloc(1, n + 1);
	fread(p, n, 1, fp);

	if (0) {
	error:
		p = NULL;
		n = 0;
	}

	if (fp)
		fclose(fp);

	if (len)
		*len = n;
	return p;
}

int
clamp(int x, int a, int b)
{
	if (x < a)
		return a;
	if (x > b)
		return b;
	return x;
}

#endif
