#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern char **environ;

int
main(int argc, char *argv[])
{
	char name[32], *ptr, *env;
	FILE *fp;
	size_t i;

	fp = NULL;
	if (argc >= 2)
		fp = fopen(argv[1], "wb");

	if (!fp)
		fp = stdout;

	fprintf(fp, "Environment Variables\n");
	for (i = 0; environ[i]; i++) {
		snprintf(name, sizeof(name), "%s", environ[i]);
		ptr = strchr(name, '=');
		if (ptr)
			*ptr = '\0';

		env = getenv(name);
		if (!env)
			env = "(nil)";

		fprintf(fp, "%s | getenv: %s\n", environ[i], env);
	}

	fclose(fp);
	return 0;
}
