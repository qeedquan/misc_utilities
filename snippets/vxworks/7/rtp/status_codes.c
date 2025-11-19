#include <stdio.h>
#include <errno.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <semLib.h>
#include <memLib.h>
#include <taskLib.h>
#include <msgQLib.h>
#include <msgQEvLib.h>
#include <edrLib.h>
#include <rtpLib.h>
#include <sdLib.h>
#include <ftpLib.h>
#include <pingLib.h>
#include <randomNumGen.h>
#include <pthread.h>

#define F(x)                        \
	{                           \
		printf("%s\n", #x); \
		x();                \
		printf("\n");       \
	}

#define P(x) printf("%s: %#x\n", #x, x);

void
objlib(void)
{
	P(S_objLib_OBJ_ID_ERROR);
	P(S_objLib_OBJ_DELETED);
	P(S_objLib_OBJ_UNAVAILABLE);
	P(S_objLib_OBJ_INVALID_ARGUMENT);
	P(S_objLib_OBJ_TIMEOUT);
	P(S_objLib_OBJ_HANDLE_TBL_FULL);
	P(S_objLib_OBJ_OPERATION_UNSUPPORTED);
	P(S_objLib_OBJ_NAME_CLASH);
	P(S_objLib_OBJ_NOT_FOUND);
	P(OM_CREATE);
	P(OM_EXCL);
	P(OM_DELETE_ON_LAST_CLOSE);
}

void
msgqlib(void)
{
	P(S_msgQLib_INVALID_MSG_LENGTH);
	P(S_msgQLib_NON_ZERO_TIMEOUT_AT_INT_LEVEL);
	P(S_msgQLib_INVALID_QUEUE_TYPE);
	P(S_msgQLib_ILLEGAL_OPTIONS);
	P(S_msgQLib_ILLEGAL_PRIORITY);
	P(S_msgQLib_UNSUPPORTED_OPERATION);
	P(S_msgQLib_INVALID_MSG_COUNT);
	P(S_msgQLib_ILLEGAL_BUFFER);
	P(MSG_PRI_NORMAL);
	P(MSG_PRI_URGENT);
	P(MSG_Q_FIFO);
}

void
eventlib(void)
{
	P(S_eventLib_TIMEOUT);
	P(S_eventLib_NOT_ALL_EVENTS);
	P(S_eventLib_ALREADY_REGISTERED);
	P(S_eventLib_EVENTSEND_FAILED);
	P(S_eventLib_ZERO_EVENTS);
	P(S_eventLib_TASK_NOT_REGISTERED);
	P(S_eventLib_NULL_TASKID_AT_INT_LEVEL);
	P(S_eventLib_INVALID_OPTIONS);
	P(S_eventLib_NULL_TASKID_AT_INT_LEVEL);
	P(S_eventLib_INVALID_OPTIONS);
}

void
semlib(void)
{
	P(S_semLib_INVALID_OPERATION);
	P(S_semLib_COUNT_OVERFLOW);
	P(S_semLib_INVALID_OPTION);
	P(S_semLib_INVALID_STATE);

	P(SEM_Q_FIFO);
	P(SEM_Q_PRIORITY);
	P(SEM_EVENTSEND_ERR_NOTIFY);
	P(SEM_INTERRUPTIBLE);
	P(SEM_TASK_DELETION_WAKEUP);
	P(SEM_DELETE_SAFE);
	P(SEM_INVERSION_SAFE);
	P(SEM_KERNEL);
	P(SEM_USER);
	P(SEM_TASK_DELETION_WAKEUP);

	P(SEM_EMPTY);
	P(SEM_FULL);

	P(SEM_TYPE_BINARY);
	P(SEM_TYPE_MUTEX);
	P(SEM_TYPE_COUNTING);
	P(SEM_TYPE_RW);
}

void
memlib(void)
{
	P(S_memLib_NOT_ENOUGH_MEMORY);
	P(S_memLib_INVALID_NBYTES);
	P(S_memLib_BLOCK_ERROR);
	P(S_memLib_NO_PARTITION_DESTROY);
	P(S_memLib_PAGE_SIZE_UNAVAILABLE);
	P(S_memLib_WRONG_PART_ID);
	P(S_memLib_INVALID_ADDRESS);
	P(S_memLib_INVALID_ALIGNMENT);
	P(S_memLib_FUNC_NOT_AVAILABLE);
	P(S_memLib_MEM_ALLOC_ERROR_NO_ALLOC_SET);
	P(S_memLib_RTP_MEM_ALLOC_ERROR_NO_ALLOC_SET);
	P(S_memLib_RTP_MEM_FREE_ERROR_NO_ALLOC_SET);
}

void
tasklib(void)
{
	P(VX_TASK_CTL_ACTIVATE);
	P(VX_TASK_CTL_SUSPEND);
	P(VX_TASK_CTL_RESUME);
	P(VX_TASK_CTL_RESTART);
	P(VX_TASK_CTL_PRI_NORMAL_GET);
	P(VX_TASK_CTL_PRIORITY_GET);
	P(VX_TASK_CTL_PRIORITY_SET);
	P(VX_TASK_CTL_VERIFY);
	P(VX_TASK_CTL_VAR_ADD);
	P(VX_TASK_CTL_VAR_DELETE);
	P(VX_TASK_CTL_VAR_GET);
	P(VX_TASK_CTL_VAR_SET);
	P(VX_TASK_CTL_TASK_EXIT);
	P(VX_TASK_CTL_UTCB_SET);
	P(VX_TASK_CTL_UTCB_GET);
	P(VX_TASK_CTL_EXIT_REGISTER);

	P(S_taskLib_ILLEGAL_PRIORITY);
	P(S_taskLib_ILLEGAL_OPERATION);
	P(S_taskLib_ILLEGAL_OPTIONS);
	P(S_taskLib_ILLEGAL_STACK_INFO);
	P(S_taskLib_NOT_ENOUGH_EXC_STACK);
	P(S_taskLib_TASK_VAR_NOT_FOUND);
}

void
randlib(void)
{
	P(RANDOM_NUM_GEN_NO_ENTROPY);
	P(RANDOM_NUM_GEN_NOT_ENOUGH_ENTROPY);
	P(RANDOM_NUM_GEN_MAX_ENTROPY);
	P(RANDOM_NUM_GEN_ENOUGH_ENTROPY);
	P(RANDOM_NUM_GEN_ERROR);
}

void
edrlib(void)
{
	P(S_edrLib_NOT_INITIALIZED);
	P(S_edrLib_PROTECTION_FAILURE);
	P(S_edrLib_INVALID_OPTION);
}

void
errors(void)
{
	P(EINTR);
	P(EDOOM);
	P(ENOSYS);
	P(EPERM);
	P(EINVAL);
	P(ENOMEM);
	P(ENOENT);
	P(EISDIR);
	P(ENOTDIR);
	P(EBADF);
	P(ENOTSUP);
	P(EALREADY);
	P(EISCONN);
	P(EINPROGRESS);
	P(EAFNOSUPPORT);
	P(EPROTOTYPE);
	P(EPROTONOSUPPORT);
	P(ENFILE);
	P(ENOTCONN);
	P(ENETUNREACH);
	P(ENETRESET);
	P(ECONNABORTED);
	P(ECONNRESET);
	P(ENOBUFS);
	P(ELOOP);
	P(EHOSTUNREACH);
	P(ENOTBLK);
	P(ECONNREFUSED);
	P(OK);
	P(ERROR);
}

void
symbolics(void)
{
	P(NO_WAIT);
	P(WAIT_FOREVER);
}

void
rtplib(void)
{
	P(S_rtpLib_INVALID_FILE);
	P(S_rtpLib_INVALID_TASK_OPTION);
	P(S_rtpLib_INSTANTIATE_FAILED);
	P(RTP_ID_ERROR);
}

void
sdlib(void)
{
	P(SD_ATTR_RW);
	P(SD_CACHE_OFF);
}

void
ftplib(void)
{
	P(FTP_COMPLETE);
}

void
pinglib(void)
{
	P(PING_OPT_SILENT);
}

void
vxlib(void)
{
	P(VX_FP_TASK);
	P(VX_NO_STACK_PROTECT);
	P(VX_NO_STACK_FILL);
	P(VX_TASK_NOACTIVATE);
}

int
main(int argc, char *argv[])
{
	F(objlib);
	F(msgqlib)
	F(eventlib);
	F(semlib);
	F(memlib);
	F(tasklib);
	F(randlib);
	F(edrlib);
	F(errors);
	F(symbolics);
	F(rtplib);
	F(sdlib);
	F(ftplib);
	F(pinglib);
	F(vxlib);

	return 0;
}
