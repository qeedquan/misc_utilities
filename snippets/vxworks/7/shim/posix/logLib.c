#include "vxWorks.h"

int (*_func_logMsg)(char *, ...);

int
logMsg(char *fmt, ...)
{
	va_list ap;
	int n;

	va_start(ap, fmt);
	n = vprintf(fmt, ap);
	va_end(ap);
	return n;
}
