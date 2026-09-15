#include <stdio.h>
#include <vxWorks.h>
#include <syscall.h>
#include <sysLib.h>
#include <tickLib.h>

void
sysrange(int type, const char *prefix, int start, int end, int (*sysargs)(int, int *))
{
	char buf[80];
	size_t bufsz;
	int narg;
	int i, rv;

	printf("%s Table\n", prefix);
	for (i = start; i <= end; i++) {
		bufsz = sizeof(buf);
		rv = syscallInfo(i, buf, &bufsz, type);
		if (rv == OK) {
			sysargs(i, &narg);
			printf("%-5d: %-24s args=%d\n", i, buf, narg);
		}
	}
	printf("\n");
}

void
sysinfo(void)
{
	sysrange(KERN_SYSCALL_NAME, "System Call", 0, 16000, syscallNumArgsGet);
	sysrange(KERN_SYSCALL_GROUP, "System Group", 0, 16000, syscallGroupNumRtnGet);
	printf("System Processor Number: %d\n", sysProcNumGet());
	printf("System Model: %s\n", sysModel());
	printf("BSP Revision: %s\n", sysBspRev());
	printf("Auxilary Clock Rate: %llu\n", (unsigned long long)sysAuxClkRateGet());
	printf("System Clock Rate: %llu\n", (unsigned long long)sysClkRateGet());
}

int
main(int argc, char *argv[])
{
	sysinfo();
	return 0;
}
