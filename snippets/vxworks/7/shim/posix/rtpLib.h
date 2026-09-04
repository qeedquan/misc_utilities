#ifndef _RTPLIB_H_
#define _RTPLIB_H_

#include "taskLibCommon.h"

typedef struct {
	int status;
	UINT32 options;
	void *entrAddr;
	TASK_ID initTaskId;
	RTP_ID parentId;
	char pathName[VX_RTP_NAME_LENGTH + 1];
	INT32 taskCnt;
	void *textStart;
	void *textEnd;
} RTP_DESC;

RTP_ID rtpSpawn(
    const char *rtpFileName,
    const char *argv[],
    const char *envp[],
    int priority,
    size_t uStackSize,
    int options,
    int taskOptions);

RTP_ID rtpInfoGet(
    RTP_ID rtpId,
    RTP_DESC *rtpStruct);

void rtpExit(int status);

STATUS vxeAbsPathGet(RTP_ID rtpid, char *buf, size_t len);

#endif
