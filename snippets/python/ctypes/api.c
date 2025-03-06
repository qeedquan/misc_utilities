#include <string.h>

typedef struct {
	int x, y, z;
	float fx, fy, fz;
	char s1[64];
	char s2[32];
} Value;

void
fill(Value *v, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		v->x = i;
		v->y = i + 2;
		v->z = i + 3;
		v->fx = 3.14 * i;
		v->fy = 2.81 * i;
		v->fz = 305.53 * i;
		strcpy(v->s1, "hello");
		strcpy(v->s2, "world");
		v++;
	}
}
