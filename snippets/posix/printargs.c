#include <stdio.h>

int
main(int argc, char *argv[])
{
	int i;

	printf("printargs: ");
	for (i = 0; i < argc; i++)
		printf("%s ", argv[i]);
	printf("\n");
	return 0;
}
