// compile with -fplan9-extensions

#include <stdio.h>

typedef struct {
	int a, b, c;
} Foo;

typedef struct {
	int x, y, z;
} Baz;

typedef struct {
	Foo;
	Baz;
} Bar;

void
f(Foo *f)
{
	printf("%d %d %d\n", f->a, f->b, f->c);
}

int
main(void)
{
	Bar b = {1, 2, 3, 4, 5, 6};

	printf("%d %d %d\n", b.a, b.b, b.c);
	printf("%d %d %d\n", b.x, b.y, b.z);
	f(&b);

	return 0;
}
