#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void
prconf(const char *path, int name)
{
	static const char *tab[] = {
	    [_PC_LINK_MAX] = "_PC_LINK_MAX",
	    [_PC_MAX_CANON] = "_PC_MAX_CANON",
	    [_PC_MAX_INPUT] = "_PC_MAX_INPUT",
	    [_PC_NAME_MAX] = "_PC_NAME_MAX",
	    [_PC_PIPE_BUF] = "_PC_PIPE_BUF",
	    [_PC_CHOWN_RESTRICTED] = "_PC_CHOWN_RESTRICTED",
	    [_PC_NO_TRUNC] = "_PC_NO_TRUNC",
	};

	long val;
	if ((val = pathconf(path, name)) < 0)
		printf("  %-22s: %s\n", tab[name], strerror(errno));
	else
		printf("  %-22s: %ld\n", tab[name], val);
}

void
prconfall(const char *path, int argc)
{
	printf("\"%s\":\n", path);
	prconf(path, _PC_LINK_MAX);
	prconf(path, _PC_MAX_CANON);
	prconf(path, _PC_MAX_INPUT);
	prconf(path, _PC_NAME_MAX);
	prconf(path, _PC_PIPE_BUF);
	prconf(path, _PC_CHOWN_RESTRICTED);
	prconf(path, _PC_NO_TRUNC);
	if (argc > 2)
		printf("\n");
}

int
main(int argc, char *argv[])
{
	if (argc < 2)
		prconfall(".", argc);
	else {
		for (int i = 1; i < argc; i++)
			prconfall(argv[i], argc);
	}
	return 0;
}
