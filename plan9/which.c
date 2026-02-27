#include <u.h>
#include <libc.h>

int aflag = 0;

void
usage(void)
{
	fprint(2, "usage: [-a] command ...\n");
	fprint(2, "  -a: print all matching pathnames of each argument\n");
	exits("usage");
}

void
which(char *cmd, char *env, char *delim)
{
	char *name, *dir, *tok, *path;
	int fd;

	name = nil;
	path = getenv(env);
	if (path == nil)
		goto out;

	name = smprint("%s/%s", path, cmd);
	if (name == nil)
		goto out;

	tok = path;
	for (tok = path; dir = strtok(tok, delim); tok = nil) {
		sprint(name, "%s/%s", dir, cmd);
		fd = open(name, OREAD);
		if (fd < 0)
			continue;

		print("%s\n", name);
		close(fd);

		if (!aflag)
			break;
	}

out:
	free(path);
	free(name);
}

void
main(int argc, char *argv[])
{
	int i;

	ARGBEGIN
	{
	case 'a':
		aflag = 1;
		break;

	case 'h':
		usage();
	}
	ARGEND;

	if (argc == 0)
		usage();

	for (i = 0; i < argc; i++) {
		which(argv[i], "path", " ");
		which(argv[i], "PATH", ":");
	}

	exits(0);
}
