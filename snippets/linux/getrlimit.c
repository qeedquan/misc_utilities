#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int
main(void)
{
	static const struct {
		int resource;
		char str[256];
	} tab[] = {
	    {RLIMIT_AS, "RLIMIT_AS"},
	    {RLIMIT_CORE, "RLIMIT_CORE"},
	    {RLIMIT_CPU, "RLIMIT_CPU"},
	    {RLIMIT_DATA, "RLIMIT_DATA"},
	    {RLIMIT_FSIZE, "RLIMIT_FSIZE"},
	    {RLIMIT_LOCKS, "RLIMIT_LOCKS"},
	    {RLIMIT_MEMLOCK, "RLIMIT_MEMLOCK"},
	    {RLIMIT_MSGQUEUE, "RLIMIT_MSGQUEUE"},
	    {RLIMIT_NICE, "RLIMIT_NICE"},
	    {RLIMIT_NOFILE, "RLIMIT_NOFILE"},
	    {RLIMIT_NPROC, "RLIMIT_NPROC"},
	    {RLIMIT_RSS, "RLIMIT_RSS"},
	    {RLIMIT_RTPRIO, "RLIMIT_RTPRIO"},
	    {RLIMIT_RTTIME, "RLIMIT_RTTIME"},
	    {RLIMIT_SIGPENDING, "RLIMIT_SIGPENDING"},
	    {RLIMIT_STACK, "RLIMIT_STACK"},
	};

	struct rlimit rlim;
	size_t i;

	for (i = 0; i < nelem(tab); i++) {
		if (getrlimit(tab[i].resource, &rlim) < 0)
			printf("%s: %s\n", tab[i].str, strerror(errno));
		else
			printf("%s: %lld %lld\n", tab[i].str, (long long)rlim.rlim_cur, (long long)rlim.rlim_max);
	}

	return 0;
}
