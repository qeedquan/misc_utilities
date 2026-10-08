#include <stdio.h>
#include <stdbool.h>

bool
bnand(bool a, bool b)
{
	return !(a && b);
}

bool
bnor(bool a, bool b)
{
	return !(a || b);
}

bool
bnot(bool a)
{
	return !a;
}

bool
band(bool a, bool b)
{
	return a && b;
}

bool
bor(bool a, bool b)
{
	return a || b;
}

bool
bxor(bool a, bool b)
{
	return (a && !b) || (!a && b);
}

void
btruth(const char *name, bool (*f)(bool, bool))
{
	bool a, b;
	int i;

	printf("%s\n", name);
	for (i = 0; i < 4; i++) {
		a = (i >> 0) & 1;
		b = (i >> 1) & 1;
		printf("%d %d | %d\n", a, b, f(a, b));
	}
	printf("\n");
}

int
main(void)
{
	btruth("nand", bnand);
	btruth("nor", bnor);
	btruth("and", band);
	btruth("or", bor);
	btruth("xor", bxor);
	return 0;
}
