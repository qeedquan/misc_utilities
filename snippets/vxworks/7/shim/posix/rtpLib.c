#include "vxWorks.h"
#include "rtpLib.h"

RTP_ID
rtpSpawn(
    const char *rtpFileName,
    const char *argv[],
    const char *envp[],
    int priority,
    size_t uStackSize,
    int options,
    int taskOptions)
{
	posix_spawn_file_actions_t fa;
	posix_spawnattr_t attr;
	pid_t pid;

	void *pargv, *penvp;
	int ret;

	eprintf("%s(priority=%d, uStackSize=%zu, options=%#x, taskOptions=%#x)\n",
	        rtpFileName, priority, uStackSize, options, taskOptions);

	if (posix_spawn_file_actions_init(&fa) != 0)
		goto error;

	if (posix_spawnattr_init(&attr) != 0)
		goto error;

	pargv = argv;
	penvp = envp;
	if (posix_spawn(&pid, rtpFileName, &fa, &attr, pargv, penvp) != 0)
		goto error;

	ret = pid;

	if (0) {
	error:
		ret = RTP_ID_ERROR;
	}

	return ret;
}

RTP_ID
rtpInfoGet(
    RTP_ID rtpId,
    RTP_DESC *rtpStruct)
{
	memset(rtpStruct, 0, sizeof(*rtpStruct));
	return rtpId;
}

void
rtpExit(int status)
{
	eprintf("(status=%d)\n", status);
	exit(status);
}

STATUS
vxeAbsPathGet(RTP_ID rtpid, char *buf, size_t len)
{
	return OK;
}