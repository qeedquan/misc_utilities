#include <stdlib.h>
#include <syslog.h>
#include <taskLib.h>

void
dumptask(TASK_ID tid)
{
	TASK_DESC td;

	if (taskInfoGet(tid, &td) != OK) {
		syslog(LOG_INFO, "Failed to get task information");
		return;
	}

	syslog(LOG_INFO, "Task %#x", tid);
	syslog(LOG_INFO, "ID                    %#x", td.td_id);
	syslog(LOG_INFO, "RTP ID                %#x", td.td_rtpId);
	syslog(LOG_INFO, "Exception Stack Base  %p", td.td_pExcStackBase);
	syslog(LOG_INFO, "Exception Stack End   %p", td.td_pExcStackEnd);
	syslog(LOG_INFO, "Exception Stack Start %p", td.td_pExcStackStart);
	syslog(LOG_INFO, "Entry Point           %p", td.td_entry);
	syslog(LOG_INFO, "Save Stack Pointer    %p", td.td_sp);
	syslog(LOG_INFO, "Stack Base            %p", td.td_pStackBase);
	syslog(LOG_INFO, "Stack End             %p", td.td_pStackEnd);
	syslog(LOG_INFO, "Options               %#x", td.td_options);
	syslog(LOG_INFO, "Priority              %#x", td.td_priority);
	syslog(LOG_INFO, "Status                %#x", td.td_status);
	syslog(LOG_INFO, "Error Status          %#x", td.td_errorStatus);
	syslog(LOG_INFO, "Stack Size            %zu", td.td_stackSize);
	syslog(LOG_INFO, "Stack Current Usage   %zu", td.td_stackCurrent);
	syslog(LOG_INFO, "Stack High Mark       %zu", td.td_stackHigh);
	syslog(LOG_INFO, "Stack Margin          %zu", td.td_stackMargin);
	syslog(LOG_INFO, "Task Name             %s", td.td_name);
	syslog(LOG_INFO, "CPU Index             %d", td.td_cpuIndex);
	syslog(LOG_INFO, "CPU Affinity          %d", td.td_affinity);
	syslog(LOG_INFO, "UID                   %d", td.td_uid);
	syslog(LOG_INFO, "GID                   %d", td.td_gid);
	syslog(LOG_INFO, "ID Verified           %d", taskIdVerify(tid));
	syslog(LOG_INFO, "TCB pointer           %p", taskWindTcbCurrent());
	syslog(LOG_INFO, "");
}

int
tasklist(void)
{
	TASK_ID *tid;
	int ntid;
	int i;

	ntid = taskIdListGet(NULL, 0);
	tid = calloc(ntid, sizeof(*tid));
	if (tid == NULL) {
		syslog(LOG_INFO, "Failed to allocate memory for task list");
		return 1;
	}

	taskIdListGet(tid, ntid);
	for (i = 0; i < ntid; i++)
		dumptask(tid[i]);
	free(tid);

	return 0;
}
