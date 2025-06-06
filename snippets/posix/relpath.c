#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <wordexp.h>
#include <threads.h>

static char reldir[PATH_MAX];

void
setreldir(const char *dir)
{
	snprintf(reldir, sizeof(reldir), "%s", dir);
}

char *
relpath(const char *name)
{
	static thread_local char path[PATH_MAX * 2];
	char *rpath;

	wordexp_t we;

	if (name == NULL)
		return NULL;

	snprintf(path, sizeof(path), "%s", name);
	if (name[0] == '/' || name[0] == '$')
		snprintf(path, sizeof(path), "%s", name);
	else
		snprintf(path, sizeof(path), "%s/%s", reldir, name);

	if (wordexp(path, &we, 0) != 0)
		return path;

	if (we.we_wordc > 0)
		snprintf(path, sizeof(path), "%s", we.we_wordv[0]);

	rpath = realpath(path, NULL);
	if (rpath)
		snprintf(path, sizeof(path), "%s", rpath);

	free(rpath);
	wordfree(&we);

	return path;
}

int
main(void)
{
	setreldir("/home");
	printf("%s\n", relpath("hax"));
	printf("%s\n", relpath("/hax$HOME"));
	printf("%s\n", relpath("$HOME"));
	printf("%s\n", relpath("/etc/../home/../etc"));

	return 0;
}
