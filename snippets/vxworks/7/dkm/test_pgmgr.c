#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernelLib.h>
#include <adrSpaceLib.h>
#include <private/pgMgrLibP.h>
#include <private/adrSpaceLibP.h>
#include <private/vmLibP.h>
#include <private/vmInvTblLibP.h>
#include <private/rtpLibP.h>
#include <private/taskMemLibP.h>
#include <private/taskLibP.h>

PAGE_MGR_ID
newpgmgr(UINT option)
{
	PAGE_MGR_ID pgmgrid;
	RTP_ID rtpid;

	pgmgrid = NULL;
	rtpid = calloc(1, sizeof(WIND_RTP));
	if (!rtpid)
		goto error;

	pgmgrid = pgMgrCreate(option);
	if (!pgmgrid)
		goto error;

	// set rtp vm context
	rtpid->kernelRtpCmn.vmContextId = pgmgrid->vmContextId;
	rtpid->kernelRtpCmn.pgMgrId = pgmgrid;

	// init rtp to say it is deletable
	RTP_DELETE_SAFE_INIT(rtpid);

	// switch to vm context
	// flag forces context switch even if the RTP is in delete state
	// this is required so RTP deletion happens in its own context
	if (!taskMemCtxSwitch(rtpid, RTP_NOT_CHECK_CTX))
		goto error;

	if (0) {
	error:
		if (pgmgrid)
			pgMgrDelete(pgmgrid);
		pgmgrid = NULL;
	}

	free(rtpid);
	return pgmgrid;
}

void
freepgmgr(PAGE_MGR_ID pgmgrid)
{
	RTP_ID rtpid;

	if (!pgmgrid)
		return;

	rtpid = taskIdCurrent->currentCtx;

	TASK_SAFE();

	// unmap the private pages
	pgMgrPrivateUnmap(pgmgrid);
	// switch back to kernel task
	taskMemCtxSwitch(kernelId, 0);
	// must not be called in owner rtp
	pgMgrDelete(pgmgrid);

	free(rtpid);
	TASK_UNSAFE();
}

STATUS
testrtp(int iterations)
{
	STATUS status;
	PAGE_MGR_ID pgmgrid;
	VIRT_ADDR va;
	PHYS_ADDR pa;
	UINT8 *ptr;
	UINT8 i;
	int n;

	status = OK;
	pgmgrid = newpgmgr(ADR_SPACE_TYPE_RTP);
	if (!pgmgrid)
		goto error;

	for (n = 0; n < iterations; n++) {
		if (!(va = pgMgrPageAlloc(pgmgrid, 16, ADR_SPACE_OPT_MAPPED | ADR_SPACE_OPT_NONCONTIG)))
			goto error;

		if (vmTranslate(pgmgrid->vmContextId, va, &pa) != OK)
			printf("Failed to translate physical address\n");

		printf("%llx %llx\n", (unsigned long long)va, (unsigned long long)pa);
		ptr = (UINT8 *)va;
		for (i = 0; i < 16; i++)
			ptr[i] = i + n;

		for (i = 0; i < 16; i++)
			printf("%d ", ptr[i]);
		printf("\n");
	}

	// the new page manager has all the mappings of the kernelId at the time we allocated a manager
	// this should show same mappings as kernelId, but with the new pages we just alloced above
	vmContextShow(pgmgrid->vmContextId);

	// this shows the kernel context
	// usually when we are in the shell and do vmContextShow()
	// this context is what we're seeing
	vmContextShow(kernelId->kernelRtpCmn.vmContextId);

	if (0) {
	error:
		status = ERROR;
	}

	freepgmgr(pgmgrid);
	return status;
}
