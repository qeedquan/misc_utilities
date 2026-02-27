#include <cstdio>

const char *shader = R"(

#version 180

void main(void) {
	mat4 A, B, C;
	A *= B;
	A -= C[1][3];
	if (A[0][0] == 10) {
		return 1
	}
}
)";

int
main()
{
	printf("%s\n", shader);
	return 0;
}
