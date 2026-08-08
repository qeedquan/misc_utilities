#include <stdio.h>
#include <stdlib.h>

void *
foo(size_t size)
{
	printf("%s(%zu)\n", __func__, size);
	return NULL;
}

// calls foo because bar was never provided
__attribute__((weak, alias("foo"))) void *bar(size_t size);

int
main(void)
{
	if (bar(20) == NULL) {
		foo(100);
		bar(200);
	}
	return 0;
}
