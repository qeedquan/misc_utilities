#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	char *line;
	size_t len;
	FILE *fp;
	int i;

	line = NULL;
	len = 0;
	for (i = 1; i < argc; i++) {
		fp = fopen(argv[i], "rt");
		if (!fp)
			continue;

		while (getline(&line, &len, fp) != -1)
			printf("%s", line);

		fclose(fp);
	}
	free(line);

	return 0;
}
