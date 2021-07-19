#include <stdio.h>
#include <taskLib.h>

void
dumptask(TASK_ID tid)
{
	TASK_DESC td;

	if (taskInfoGet(tid, &td) != OK) {
		printf("Failed to get task information\n");
		return;
	}

	printf("Task %#x\n", tid);
	printf("ID                    %#x\n", td.td_id);
	printf("RTP ID                %#x\n", td.td_rtpId);
	printf("Exception Stack Base  %p\n", td.td_pExcStackBase);
	printf("Exception Stack End   %p\n", td.td_pExcStackEnd);
	printf("Exception Stack Start %p\n", td.td_pExcStackStart);
	printf("Entry Point           %p\n", td.td_entry);
	printf("Save Stack Pointer    %p\n", td.td_sp);
	printf("Stack Base            %p\n", td.td_pStackBase);
	printf("Stack End             %p\n", td.td_pStackEnd);
	printf("Options               %#x\n", td.td_options);
	printf("Priority              %#x\n", td.td_priority);
	printf("Status                %#x\n", td.td_status);
	printf("Error Status          %#x\n", td.td_errorStatus);
	printf("Stack Size            %zu\n", td.td_stackSize);
	printf("Stack Current Usage   %zu\n", td.td_stackCurrent);
	printf("Stack High Mark       %zu\n", td.td_stackHigh);
	printf("Stack Margin          %zu\n", td.td_stackMargin);
	printf("Task Name             %s\n", td.td_name);
	printf("CPU Index             %d\n", td.td_cpuIndex);
	printf("CPU Affinity          %d\n", td.td_affinity);
	printf("UID                   %d\n", td.td_uid);
	printf("GID                   %d\n", td.td_gid);
}

int
main(void)
{
	TASK_ID tid;

	tid = taskIdSelf();
	dumptask(tid);

	return 0;
}
