#ifndef _TASKLIB_H_
#define _TASKLIB_H_

#include "taskLibCommon.h"

struct windTcb {
	int _;
};

#define TASK_ID_ERROR -1

#define S_taskLib_NAME_NOT_FOUND (M_taskLib | 101)
#define S_taskLib_TASK_HOOK_TABLE_FULL (M_taskLib | 102)
#define S_taskLib_TASK_HOOK_NOT_FOUND (M_taskLib | 103)
#define S_taskLib_TASK_SWAP_HOOK_REFERENCED (M_taskLib | 104)
#define S_taskLib_TASK_SWAP_HOOK_SET (M_taskLib | 105)
#define S_taskLib_TASK_SWAP_HOOK_CLEAR (M_taskLib | 106)
#define S_taskLib_TASK_VAR_NOT_FOUND (M_taskLib | 107)
#define S_taskLib_TASK_UNDELAYED (M_taskLib | 108)
#define S_taskLib_ILLEGAL_PRIORITY (M_taskLib | 109)
#define S_taskLib_ILLEGAL_OPTIONS (M_taskLib | 110)
#define S_taskLib_NO_TCB (M_taskLib | 111)
#define S_taskLib_ILLEGAL_OPERATION (M_taskLib | 112)
#define S_taskLib_ILLEGAL_STACK_INFO (M_taskLib | 113)
#define S_taskLib_STOPPED_DELETE_SAFE_TASK (M_taskLib | 114)
#define S_taskLib_NOT_ENOUGH_EXC_STACK (M_taskLib | 115)

#define TASK_ID_NULL (0)

TASK_ID taskOpen(
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
    _Vx_usr_arg_t arg10);

TASK_ID taskSpawn(
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
    _Vx_usr_arg_t arg10);

TASK_ID taskCreate(
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
    _Vx_usr_arg_t arg10);

STATUS taskPrioritySet(TASK_ID tid, int newprio);
STATUS taskPriorityGet(TASK_ID tid, int *prio);

STATUS taskActivate(TASK_ID tid);

STATUS taskClose(TASK_ID tid);
STATUS taskDelete(TASK_ID tid);

STATUS taskDelay(_Vx_ticks_t ticks);
STATUS taskDelayEx(_Vx_ticks_t ticks, UINT32 flags);

void taskExit(_Vx_exit_code_t code);

STATUS taskWait(TASK_ID tid, _Vx_ticks_t timeout);

char *taskName(TASK_ID tid);
STATUS taskNameGet(TASK_ID tid, char *buf, size_t len);

STATUS taskInfoGet(TASK_ID tid, TASK_DESC *desc);

STATUS taskIdVerify(TASK_ID tid);
TASK_ID taskIdSelf(void);

STATUS taskSafe(void);
STATUS taskUnsafe(void);

STATUS taskCtl(TASK_ID tid, VX_TASK_CTL_CMD command, void *pArg, size_t *pArgSize);

struct windTcb *taskWindTcbCurrent(void);

BOOL taskIsSuspended(TASK_ID tid);

#endif
