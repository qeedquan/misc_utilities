#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <vxWorks.h>
#include <msgQLib.h>
#include <semLib.h>
#include "util.h"

static SEM_ID printlock;

static void
initprint(void)
{
	printlock = semBCreate(SEM_Q_FIFO, SEM_FULL);
	assert(printlock != SEM_ID_NULL);
}

void
initutil(void)
{
	initprint();
}

int
xprintf(const char *fmt, ...)
{
	va_list ap;
	int n;

	semTake(printlock, WAIT_FOREVER);
	va_start(ap, fmt);
	n = vprintf(fmt, ap);
	va_end(ap);
	semGive(printlock);
	return n;
}

void
hexdump(void *buf, size_t len)
{
	unsigned char *p;
	size_t i;

	p = buf;
	for (i = 0; i < len; i++) {
		printf("%02x ", p[i]);
		if ((i & 15) == 15)
			printf("\n");
	}
	printf("\n");
}
