#include <stdio.h>

void Fido(void);

__declspec(dllexport)
void
SayHello(const char *name)
{
	printf("Hello %s\n", name);
}

__declspec(dllexport)
void
Question(const char *str, int val)
{
	printf("%s(str = %s, val = %d)\n", __func__, str, val);
	Fido();
}
