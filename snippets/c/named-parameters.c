#include <stdio.h>
#include <stdarg.h>

typedef struct {
	int x, y;
	char msg[32];
} Args;

#define calc(...) _calc((Args){ .x = 1, .y = 2, __VA_ARGS__ })

void
_calc(Args a)
{
	printf("x: %d y: %d msg: %s\n", a.x, a.y, a.msg);
}

int
main()
{
	calc(.x = 5, .y = 10, .msg = "hello");
	calc(.msg = "ZYX");
	calc();

	return 0;
}
