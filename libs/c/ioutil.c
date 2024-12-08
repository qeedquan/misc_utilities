#include "ioutil.h"

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

void *
xmalloc(size_t size)
{
	void *p;

	if (size == 0)
		size = 1;

	p = malloc(size);
	if (!p)
		abort();
	return p;
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

void *
xrealloc(void *ptr, size_t size)
{
	if (size == 0)
		size = 1;

	ptr = realloc(ptr, size);
	if (!ptr)
		abort();
	return ptr;
}

void *
readfile(const char *name, size_t *len)
{
	FILE *fp;
	void *p;
	long n;

	p = NULL;
	fp = fopen(name, "rb");
	if (!fp)
		goto error;

	fseek(fp, 0, SEEK_END);
	n = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (n < 0)
		goto error;

	p = calloc(1, n + 1);
	if (!p)
		goto error;

	if (fread(p, n, 1, fp) != 1)
		goto error;

	if (0) {
	error:
		free(p);
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
writefile(const char *name, const void *buf, size_t len)
{
	FILE *fp;
	int r;

	r = 0;
	fp = fopen(name, "wb");
	if (!fp)
		goto error;

	if (fwrite(buf, len, 1, fp) != 1)
		goto error;

	if (0) {
	error:
		r = -errno;
	}

	if (fp)
		fclose(fp);

	return r;
}

void *
freadall(FILE *fp, size_t *len)
{
	char *p, *q;
	size_t l, n, m;

	p = malloc(IOUNIT + 1);
	if (!p)
		goto error;

	n = 0;
	m = IOUNIT;
	for (;;) {
		l = fread(p + n, 1, m - n, fp);
		if (l == 0)
			break;

		n += l;
		if (n >= m) {
			m *= 2;
			q = realloc(p, m + 1);
			if (!q)
				goto error;
			p = q;
		}
	}
	p[n] = '\0';

	if (0) {
	error:
		free(p);
		p = NULL;
		n = 0;
	}

	if (len)
		*len = n;

	return p;
}
