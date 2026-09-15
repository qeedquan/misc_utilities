#include <vxWorks.h>
#include <stdio.h>
#include <ioLib.h>

void
limits(void)
{
	static const struct {
		const char *name;
		int resource;
	} tab[] = {
	    {"RLIMIT_NOFILE", RLIMIT_NOFILE},
	    {"RLIMIT_STACK", RLIMIT_STACK},
	    {"RLIMIT_CPU", RLIMIT_CPU},
	    {"RLIMIT_FSIZE", RLIMIT_FSIZE},
	    {"RLIMIT_DATA", RLIMIT_DATA},
	};

	struct rlimit rl;
	size_t i;

	for (i = 0; i < NELEMENTS(tab); i++) {
		if (getrlimit(tab[i].resource, &rl) < 0) {
			printf("Failed to get limit for %s\n", tab[i].name);
			continue;
		}
		printf("%s cur %ld max %ld\n", tab[i].name, (long)rl.rlim_cur, (long)rl.rlim_max);
	}
}

int
main(int argc, char *argv[])
{
	limits();
	return 0;
}
