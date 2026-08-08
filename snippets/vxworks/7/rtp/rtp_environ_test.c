#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <rtpLib.h>

int
main(int argc, char *argv[])
{
	RTP_ID rtpid[10];
	char store[32], buf[32];
	const char *args[3] = {"printenv.vxe", store, NULL};
	const char *env[] = { buf, NULL };
	unsigned i;

	for (i = 0; i < NELEMENTS(rtpid); i++) {
		snprintf(buf, sizeof(buf), "env_%u=%u", i, i);
		snprintf(store, sizeof(store), "/ram0/env_%u.txt", i);

		rtpid[i] = rtpSpawn(args[0], args, env, 100, 0x10000, 0, 0);
	}
	while (wait(NULL) >= 0)
		;

	return 0;
}
