#include <stdio.h>

int add(int, int);
void hello(void);
int return87(void);

int
main(void)
{
	hello();
	printf("return87: %d\n", return87());
	printf("add: %d\n", add(32, 10));

	return 0;
}
