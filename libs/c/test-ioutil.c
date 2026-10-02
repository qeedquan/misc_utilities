#include <assert.h>
#include "ioutil.h"

int
main(void)
{
	FILE *f = fopen("test-ioutil.c", "rb");
	assert(f);

	char *p;
	size_t n;
	p = freadall(f, &n);
	assert(p);

	printf("%zu\n%s", n, p);
	
	fclose(f);
	free(p);

	char *q = readfile("test-ioutil.c", &n);
	assert(q);
	printf("%zu\n%s\n", n, q);
	free(q);

	return 0;
}
