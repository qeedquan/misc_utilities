#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <rtpLib.h>
#include <errnoLib.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

void
usage(void)
{
	printf("usage: uid gid euid egid rtp.vxe ...\n");
	exit(2);
}

void
procinfo(void)
{
	gid_t groups[32];
	int i, n;

	printf("---------------\n");
	printf("Process info\n");
	printf("pid  %d\n", getpid());
	printf("ppid %d\n", getppid());
	printf("uid  %d\n", getuid());
	printf("gid  %d\n", getgid());
	printf("euid %d\n", geteuid());
	printf("egid %d\n", getegid());
	printf("\n");

	n = getgroups(nelem(groups), groups);
	printf("groups - \n");
	for (i = 0; i < n; i++)
		printf("%d ", groups[i]);
	printf("\n");
	printf("---------------\n");
}

void
launch(uid_t uid, gid_t gid, uid_t euid, gid_t egid, const char *rtp)
{
	const char *argv[] = {rtp, NULL};
	int r;

	printf("Trying to launch %s with uid %d gid %d euid %d egid %d\n", rtp, uid, gid, euid, egid);

	r = seteuid(euid);
	if (r < 0)
		printf("Failed to set EUID: %s\n", strerror(errno));

	r = setegid(egid);
	if (r < 0)
		printf("Failed to set EGID: %s\n", strerror(errno));

	r = setuid(uid);
	if (r < 0)
		printf("Failed to set UID: %s\n", strerror(errno));

	r = setgid(gid);
	if (r < 0)
		printf("Failed to set GID: %s\n", strerror(errno));

	r = rtpSpawn(rtp, argv, NULL, 100, 0x10000, 0, 0);
	if (r == RTP_ID_ERROR)
		printf("Failed to launch RTP: %s\n", strerror(errnoGet()));
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc == 2 && !strcmp(argv[1], "-h"))
		usage();

	procinfo();
	for (i = 1; i + 4 < argc; i += 5)
		launch(atoi(argv[i]), atoi(argv[i + 1]), atoi(argv[i + 2]), atoi(argv[i + 3]), argv[i + 4]);

	return 0;
}
