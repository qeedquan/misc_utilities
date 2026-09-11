#define _GNU_SOURCE
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int
main(void)
{
	intmax_t pid, ppid, pgrp, sid, gid;
	char domain[256], *login;

	pid = getpid();
	ppid = getppid();
	pgrp = getpgrp();
	gid = getgid();
	sid = getsid(pid);
	if (getdomainname(domain, sizeof(domain)) < 0)
		strcpy(domain, "(none)");
	if ((login = getlogin()) == NULL)
		login = "(null)";

	printf("pid    %jd\n", pid);
	printf("ppid   %jd\n", ppid);
	printf("gid    %jd\n", gid);
	printf("sid    %jd\n", sid);
	printf("prgp   %jd\n", pgrp);
	printf("domain %s\n", domain);
	printf("login  %s\n", login);

	return 0;
}
