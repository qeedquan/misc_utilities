#include "vxWorks.h"

void
bcopyWords(const void *source, void *destination, size_t nwords)
{
	memmove(destination, source, nwords * 2);
}

void
bcopyLongs(const void *source, void *destination, size_t nlongs)
{
	memmove(destination, source, nlongs * 4);
}
