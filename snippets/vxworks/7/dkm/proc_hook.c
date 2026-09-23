#include <stdlib.h>
#include <vxWorks.h>
#include <syslog.h>
#include <rtpLib.h>
#include <taskLib.h>
#include <taskHookLib.h>
#include <errnoLib.h>

typedef unsigned long long uvlong;

int allow;

STATUS
rtp_prehook(const char *name, const char *argv[], const char *envp[], int priority, size_t stacksize, int options, int task_options)
{
	syslog(LOG_INFO, "%s(name = %s, priority = %d, argv = %p, envp = %p, stacksize = %zu, options = %d, task_options = %d",
	       __func__, name, argv, envp, priority, stacksize, options, task_options);
	return (allow) ? OK : ERROR;
}

STATUS
rtp_posthook(const RTP_ID rid)
{
	syslog(LOG_INFO, "%s(rid = %llu)", __func__, (uvlong)(uintptr_t)rid);
	return OK;
}

void
rtp_init_complete(const RTP_ID rid)
{
	syslog(LOG_INFO, "%s(rid = %llu)", __func__, (uvlong)(uintptr_t)rid);
}

int
task_create_hook(WIND_TCB *tcb)
{
	syslog(LOG_INFO, "%s(tcb = %p)", __func__, tcb);
	if (!allow) {
		errnoOfTaskSet(taskIdSelf(), EPERM);
		return ERROR;
	}
	return OK;
}

int
task_delete_hook(void)
{
	syslog(LOG_INFO, "%s()", __func__);
	return OK;
}

int
procmon(int arg1)
{
	allow = arg1;

	syslog(LOG_INFO, "RTP/Task Hooking launched (allow = %d)", allow);
	rtpPreCreateHookAdd(rtp_prehook, TRUE);
	rtpPostCreateHookAdd(rtp_posthook, TRUE);
	rtpInitCompleteHookAdd(rtp_init_complete, TRUE);
	taskCreateHookAdd(task_create_hook);
	taskDeleteHookAdd(task_delete_hook);

	return 0;
}
