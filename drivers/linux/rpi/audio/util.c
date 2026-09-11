#include "libc.h"
#include "util.h"

size_t
pgroundup(size_t size)
{
	size_t pagesize;

	pagesize = getpagesize();
	return ((size + pagesize - 1) / pagesize) * pagesize;
}

void
millisleep(long ms)
{
	struct timespec tp;

	tp.tv_sec = ms / 1000;
	tp.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&tp, NULL);
}

void *
readfile(const char *name, size_t *size)
{
	FILE *fp;
	void *data;
	long len;

	data = NULL;
	fp = fopen(name, "rb");
	if (!fp)
		goto error;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (len < 0)
		goto error;

	data = calloc(1, len);
	if (!data)
		goto error;

	if (fread(data, len, 1, fp) != 1)
		goto error;

	*size = len;
	return data;

error:
	if (fp)
		fclose(fp);
	free(data);
	*size = 0;
	return NULL;
}
