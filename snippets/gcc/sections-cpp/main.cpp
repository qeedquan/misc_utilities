#include "section.h"

__attribute__((section(".f_fixed")))
void fixed()
{
	printf("%s:%d\n", __func__, __LINE__);
}

void foo()
{
	__asm__("callq 0x95000\n");
}

int main()
{
	Data data = {};
	Far f(NULL, 0);
	load(&data, sizeof(data));

	__attribute__((ms_abi)) int (*fp)(const char *, const char *, int, int, float);
	fp = quux;
	printf("%d\n", fp("fff", "ggg", 1, 2, 3));
	printf("%p %p %p %p\n", main, quux, fp, load);

	fixed();
	for (auto i = 0; i < 8; i++)
		foo();
	__asm__("callq 0x95000\n");
	
	return 0;
}
