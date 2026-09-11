#include "vxWorks.h"

typedef struct {
	pthread_t tid;
	pthread_attr_t attr;
	TASK_DESC desc;

	_Vx_usr_arg_t args[10];

	int active;
} TD;

static pthread_mutex_t tdlk = PTHREAD_MUTEX_INITIALIZER;
static TD tds[128];
static size_t ntd;

static void
tdlock(void)
{
	pthread_mutex_lock(&tdlk);
}

static void
tdunlock(void)
{
	pthread_mutex_unlock(&tdlk);
}

static void *
tdcall(void *arg)
{
	TD *td;
	uintptr_t i;

	i = (uintptr_t)arg;
	td = tds + i;
	td->desc.td_entry(td->args[0], td->args[1], td->args[2], td->args[3], td->args[4], td->args[5], td->args[6], td->args[7], td->args[8], td->args[9]);

	return NULL;
}

static TASK_ID
mkthread(const char *name,
         int options,
         int mode,
         char *pStackBase,
         int stackSize,
         FUNCPTR entryPt,
         _Vx_usr_arg_t arg1,
         _Vx_usr_arg_t arg2,
         _Vx_usr_arg_t arg3,
         _Vx_usr_arg_t arg4,
         _Vx_usr_arg_t arg5,
         _Vx_usr_arg_t arg6,
         _Vx_usr_arg_t arg7,
         _Vx_usr_arg_t arg8,
         _Vx_usr_arg_t arg9,
         _Vx_usr_arg_t arg10)
{
	pthread_t tid;
	TD *td;
	int r;

	if (options & VX_TASK_NOACTIVATE) {
		r = EINVAL;
		goto error;
	}

	if ((mode & (OM_CREATE | OM_EXCL)) != (OM_CREATE | OM_EXCL)) {
		r = EPERM;
		goto error;
	}

	if (pStackBase != NULL) {
		r = ENOMEM;
		goto error;
	}

	if (!name)
		name = "anon";

	td = tds + ntd + 1;
	memset(td, 0, sizeof(*td));
	snprintf(td->desc.td_name, sizeof(td->desc.td_name), "%s", name);
	td->desc.td_id = ntd + 1;
	td->desc.td_stackSize = stackSize;
	td->desc.td_entry = entryPt;
	td->args[0] = arg1;
	td->args[1] = arg2;
	td->args[2] = arg3;
	td->args[3] = arg4;
	td->args[4] = arg5;
	td->args[5] = arg6;
	td->args[6] = arg7;
	td->args[7] = arg8;
	td->args[8] = arg9;
	td->args[9] = arg10;

	pthread_attr_init(&td->attr);
	pthread_attr_setstacksize(&td->attr, stackSize);
	if ((r = pthread_create(&tid, &td->attr, tdcall, (void *)(ntd + 1))) != 0)
		goto error;

	eprintf("Created thread %s\n", td->desc.td_name);

	td->tid = tid;
	td->active = 1;

	return ++ntd;

error:
	eprintf("Failed to create thread: %s\n", strerror(r));
	return TASK_ID_NULL;
}

TASK_ID
taskOpen(
    const char *name,
    int priority,
    int options,
    int mode,
    char *pStackBase,
    size_t stackSize,
    void *context,
    FUNCPTR entryPt,
    _Vx_usr_arg_t arg1,
    _Vx_usr_arg_t arg2,
    _Vx_usr_arg_t arg3,
    _Vx_usr_arg_t arg4,
    _Vx_usr_arg_t arg5,
    _Vx_usr_arg_t arg6,
    _Vx_usr_arg_t arg7,
    _Vx_usr_arg_t arg8,
    _Vx_usr_arg_t arg9,
    _Vx_usr_arg_t arg10)
{
	int ret;

	eprintf("(name=%s, priority=%d, options=%d, mode=%d, pStackBase=%p, stackSize=%zu, context=%p)\n",
	        name, priority, options, mode, pStackBase, stackSize, context);

	tdlock();
	ret = mkthread(name, options, mode, pStackBase, stackSize, entryPt,
	               arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tdunlock();
	return ret;
}

TASK_ID
taskSpawn(
    char *name,
    int priority,
    int options,
    size_t stackSize,
    FUNCPTR entryPt,
    _Vx_usr_arg_t arg1,
    _Vx_usr_arg_t arg2,
    _Vx_usr_arg_t arg3,
    _Vx_usr_arg_t arg4,
    _Vx_usr_arg_t arg5,
    _Vx_usr_arg_t arg6,
    _Vx_usr_arg_t arg7,
    _Vx_usr_arg_t arg8,
    _Vx_usr_arg_t arg9,
    _Vx_usr_arg_t arg10)
{
	int ret;

	eprintf("(name=%s, priority=%d, options=%#x, stackSize=%zu, entryPt=%lx)\n", name, priority, options, stackSize, (uintptr_t)entryPt);

	tdlock();
	ret = mkthread(name, options, OM_CREATE | OM_EXCL, NULL, stackSize, entryPt, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	tdunlock();
	return ret;
}

STATUS
taskActivate(TASK_ID tid)
{
	TD *td;
	int ret;

	ret = OK;
	tdlock();

	if (tid < 0 || tid > ntd) {
		ret = ERROR;
		goto out;
	}

	td = tds + tid;
	if (td->active)
		goto out;

	if (pthread_create(&td->tid, &td->attr, tdcall, (void *)(uintptr_t)tid) != 0) {
		ret = ERROR;
		goto out;
	}
	td->active = 1;

out:
	tdunlock();
	return ret;
}

TASK_ID
taskCreate(
    char *name,
    int priority,
    int options,
    size_t stackSize,
    FUNCPTR entryPt,
    _Vx_usr_arg_t arg1,
    _Vx_usr_arg_t arg2,
    _Vx_usr_arg_t arg3,
    _Vx_usr_arg_t arg4,
    _Vx_usr_arg_t arg5,
    _Vx_usr_arg_t arg6,
    _Vx_usr_arg_t arg7,
    _Vx_usr_arg_t arg8,
    _Vx_usr_arg_t arg9,
    _Vx_usr_arg_t arg10)
{
	return ERROR;
}

STATUS
taskClose(TASK_ID tid)
{
	eprintf("(tid=%d)\n", tid);
	return OK;
}

STATUS
taskDelete(TASK_ID tid)
{
	eprintf("(tid=%d)\n", tid);
	return OK;
}

void
taskExit(_Vx_exit_code_t code)
{
	eprintf("(code=%d)\n", code);
	pthread_exit(NULL);
}

STATUS
taskDelay(_Vx_ticks_t ticks)
{
	return taskDelayEx(ticks, 0);
}

STATUS
taskDelayEx(_Vx_ticks_t ticks, UINT32 flags)
{
	struct timespec tp;
	uint64_t ms;

	ms = tick_to_millisecond(ticks);
	tp = millisecond_to_timespec(ms);
	clock_nanosleep(CLOCK_MONOTONIC, 0, &tp, NULL);

	return OK;
}

STATUS
taskWait(TASK_ID tid, _Vx_ticks_t timeout)
{
	TD *td;

	if (timeout != WAIT_FOREVER)
		assert(0);

	td = NULL;
	tdlock();
	if (tid >= 0 && tid <= ntd)
		td = tds + tid;
	tdunlock();

	if (td == NULL)
		return ERROR;

	pthread_join(td->tid, NULL);
	return OK;
}

char *
taskName(TASK_ID tid)
{
	TD *td;

	td = NULL;
	tdlock();
	if (tid >= 0 && tid <= ntd)
		td = tds + tid;
	tdunlock();

	return (td) ? td->desc.td_name : NULL;
}

STATUS
taskNameGet(TASK_ID tid, char *buf, size_t len)
{
	TD *td;

	td = NULL;
	tdlock();
	if (tid >= 0 && tid <= ntd)
		td = tds + tid;
	tdunlock();

	if (!td)
		return ERROR;

	snprintf(buf, len, "%s", td->desc.td_name);
	return OK;
}

STATUS
taskInfoGet(TASK_ID tid, TASK_DESC *desc)
{
	TD *td;

	memset(desc, 0, sizeof(*desc));

	td = NULL;
	tdlock();
	if (tid >= 0 && tid <= ntd)
		td = tds + tid;
	tdunlock();

	if (!td)
		return ERROR;

	memcpy(desc, &td->desc, sizeof(*desc));
	return OK;
}

STATUS
taskIdVerify(TASK_ID tid)
{
	STATUS ret;

	ret = ERROR;

	tdlock();
	if (tid < ntd)
		ret = OK;
	tdunlock();

	return ret;
}

TASK_ID
taskIdSelf(void)
{
	pthread_t self;
	TD *td;
	TASK_ID id;
	size_t i;

	id = TASK_ID_NULL;
	self = pthread_self();

	tdlock();
	for (i = 0; i < ntd; i++) {
		td = tds + i;
		if (pthread_equal(self, td->tid)) {
			id = i;
			break;
		}
	}
	tdunlock();

	return id;
}

STATUS
taskSafe(void)
{
	return OK;
}

STATUS
taskUnsafe(void)
{
	return OK;
}

struct windTcb *
taskWindTcbCurrent(void)
{
	return NULL;
}

STATUS
taskCtl(TASK_ID tid, VX_TASK_CTL_CMD command, void *pArg, size_t *pArgSize)
{
	return OK;
}

STATUS
taskPrioritySet(TASK_ID tid, int newprio)
{
	return OK;
}

STATUS
taskPriorityGet(TASK_ID tid, int *prio)
{
	*prio = 0;
	return OK;
}

BOOL
taskIsSuspended(TASK_ID tid)
{
	return FALSE;
}
