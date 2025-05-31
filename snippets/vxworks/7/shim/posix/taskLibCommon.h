#ifndef _TASKLIB_COMMON_H_
#define _TASKLIB_COMMON_H_

#include "types/vxTypes.h"
#include "eventLibCommon.h"
#include "rtpLibCommon.h"
#include <sched.h>

#define VX_TASK_NAME_LENGTH 31

typedef int TASK_ID;

typedef cpu_set_t cpuset_t;

typedef struct {
	TASK_ID td_id;
	RTP_ID td_rtpId;
	char *td_pExcStackBase;
	char *td_pExcStackEnd;
	char *td_pExcStackStart;
	FUNCPTR td_entry;
	char *td_sp;
	char *td_pStackBase;
	char *td_pStackEnd;
	int td_options;
	int td_priority;
	UINT td_status;
	int td_errorStatus;
	size_t td_stackSize;
	size_t td_stackCurrent;
	size_t td_stackHigh;
	ssize_t td_stackMargin;
	int td_delay;
	EVENTS_DESC td_events;
	char td_name[VX_TASK_NAME_LENGTH + 1];
	size_t td_excStackSize;
	size_t td_excStackHigh;
	size_t td_excStackMargin;
	size_t td_excStackCurrent;
	int td_cpuIndex;
	cpuset_t td_affinity;
	uid_t td_uid;
	gid_t td_gid;
} TASK_DESC;

enum {
	VX_PRIVATE_ENV = 0x0080,
	VX_NO_STACK_FILL = 0x0100,
	VX_PRIVATE_UMASK = 0x0400,
	VX_TASK_NOACTIVATE = 0x2000,
	VX_NO_STACK_PROTECT = 0x4000,

	VX_USR_TASK_OPTIONS_BASE = VX_PRIVATE_ENV |
	                           VX_NO_STACK_FILL |
	                           VX_TASK_NOACTIVATE |
	                           VX_NO_STACK_PROTECT |
	                           VX_PRIVATE_UMASK,
};

typedef enum {
	VX_TASK_CTL_ACTIVATE,
	VX_TASK_CTL_DELETE,
	VX_TASK_CTL_SUSPEND,
	VX_TASK_CTL_RESUME,
	VX_TASK_CTL_RESTART,
	VX_TASK_CTL_RESET,
	VX_TASK_CTL_PRI_NORMAL_GET,
	VX_TASK_CTL_PRIORITY_GET,
	VX_TASK_CTL_PRIORITY_SET,
	VX_TASK_CTL_VERIFY,
	VX_TASK_CTL_VAR_ADD,
	VX_TASK_CTL_VAR_DELETE,
	VX_TASK_CTL_VAR_GET,
	VX_TASK_CTL_VAR_SET,
	VX_TASK_CTL_TASK_EXIT,
	VX_TASK_CTL_UTCB_SET,
	VX_TASK_CTL_UTCB_GET,
	VX_TASK_CTL_EXIT_REGISTER,
	VX_TASK_CTL_PX_ATTR_SET,
	VX_TASK_CTL_PX_ATTR_GET,
	VX_TASK_CTL_PX_ATTR_CREATE,
	VX_TASK_CTL_CHK_SCHEDULE,
	VX_TASK_CTL_STACKSIZE_GET,
	VX_TASK_CTL_CPU_AFFINITY_GET,
	VX_TASK_CTL_CPU_AFFINITY_SET,
	VX_TASK_CTL_ATOMIC_OP,
	VX_TASK_CTL_TASKNAME_TO_ID,
	VX_TASK_CTL_WAIT,
	VX_TASK_CTL_DOOM,
	VX_TASK_CTL_SET_TLS_BASE,
} VX_TASK_CTL_CMD;

// task stack allocation options
enum {
	STACK_DOWN = 0x1,
	STACK_UP = 0x2,
	EXECUTION_STACK = 0x4,
	EXCEPTION_STACK = 0x8,
};

enum {
	RTP_NOT_CHECK_CTX = 0x1,
};

#endif
