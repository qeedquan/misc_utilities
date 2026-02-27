#include "errnoLib.h"

int
errnoGet(void)
{
	return errno;
}

void
errnoSet(int code)
{
	errno = code;
}