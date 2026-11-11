/*

The QEMU PPCE500 machine is based on the MPC8544EEC
https://www.nxp.com/docs/en/data-sheet/MPC8544EEC.pdf
https://www.nxp.com/docs/en/reference-manual/MPC8544ERM.pdf

*/

#include <vxWorks.h>
#include <stdio.h>
#include <stdarg.h>
#include <pmapLib.h>
#include <vmLib.h>
#include <hwif/vxBus.h>

char *ccsrbase;
char *uartbase;

void
ppce500init(void)
{
	ccsrbase = pmapGlobalMap(0xfe0000000, 0x40000, MMU_ATTR_SUP_RW | MMU_ATTR_CACHE_OFF | MMU_ATTR_CACHE_GUARDED);
	uartbase = ccsrbase + 0x4500;
}

uint8_t
rd8(void *addr)
{
	return vxbRead8((void *)VXB_HANDLE_ORDERED, addr);
}

void
wr8(void *addr, uint8_t value)
{
	vxbWrite8((void *)VXB_HANDLE_ORDERED, addr, value);
}

void
uartputc(int c)
{
	enum {
		THR = 0x00,
		LSR = 0x05,

		LSR_THRE = 0x20,
	};

	while (!(rd8(uartbase + LSR) & LSR_THRE))
		;
	wr8(uartbase + THR, c);
}

void
earlyprint(const char *fmt, ...)
{
	char buf[256];
	va_list ap;
	size_t i;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	for (i = 0; buf[i]; i++) {
		if (buf[i] == '\n')
			uartputc('\r');
		uartputc(buf[i]);
	}
}
