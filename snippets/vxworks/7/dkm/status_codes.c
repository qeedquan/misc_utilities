#include <stdio.h>
#include <scMemVal.h>

#define F(x)                        \
	{                           \
		printf("%s\n", #x); \
		x();                \
		printf("\n");       \
	}

#define P(x) printf("%s: %#x\n", #x, x);

void
sclib(void)
{
	P(SC_PROT_READ);
	P(SC_PROT_WRITE);
	P(SC_PROT_ATOMIC_RMW);
}

void
print_status_codes(void)
{
	F(sclib);
}
