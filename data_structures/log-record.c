#include <stdio.h>
#include <stdarg.h>

typedef struct {
	char *buf;
	size_t len;
	size_t cap;

	unsigned long nwrites;
	unsigned long miswrites;
} Recording;

void
recording_init(Recording *r, char *buf, size_t cap)
{
	r->buf = buf;
	r->cap = cap;
	r->len = 0;
	r->nwrites = 0;
	r->miswrites = 0;
}

void
recording_logf(Recording *r, const char *fmt, ...)
{
	va_list ap;
	size_t n;

	va_start(ap, fmt);
	n = vsnprintf(r->buf + r->len, r->cap - r->len, fmt, ap);
	va_end(ap);

	if (r->cap - r->len < n) {
		r->len = r->cap;
		r->miswrites += 1;
	} else
		r->len += n + 1;

	r->nwrites += 1;
}

void
recording_dump(Recording *r)
{
	size_t i;
	int n;

	printf("STATS:\n");
	printf("writes %lu\n", r->nwrites);
	printf("miswrites %lu\n", r->miswrites);
	printf("\n");
	printf("LOG:\n");
	for (i = 0; i < r->len;) {
		n = printf("%s", r->buf + i);
		i += n + 1;
	}
}

int
main()
{
	Recording r;
	char buf[128];

	recording_init(&r, buf, sizeof(buf));
	recording_logf(&r, "hello world!\n");
	recording_logf(&r, "%d %d %d\n", 1, 2, 3);
	recording_logf(&r, "tau %f\n", 2.5);
	recording_logf(&r, "tso %f\n", 2.1);
	recording_dump(&r);

	return 0;
}
