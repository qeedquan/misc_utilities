#include "section.h"

Errno load(Data *data, int slot)
{
	write(1, "load\n", 5);
	return E0;
}

int quux(const char *a, const char *b, int x, int y, float f)
{
	printf("%s %s %d %d %f\n", a, b, x, y, f);
	return (x + y) * f;
}

Far::Far(void *buf, uint64_t buflen)
{
	printf("%p %ju\n", buf, (uintmax_t)buflen);
}

void Far::faze()
{
	Far f(NULL, 0);
}
