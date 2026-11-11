#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <clockLib.h>
#include <msgQLib.h>
#include <taskLib.h>
#include <semLib.h>
#include <sdLib.h>
#include <regs.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <hwif/util/vxbIsrDeferLib.h>

#define P(x) printf("%-32s: %zu bytes\n", #x, sizeof(x));

void
print_sizeof(void)
{
	printf("KERNEL SIZEOF\n");
	P(BOOL);
	P(STATUS);
	P(VIRT_ADDR);
	P(PHYS_ADDR);
	P(bool);
	P(char);
	P(short);
	P(int);
	P(long);
	P(long long);
	P(float);
	P(double);
	P(long double);
	P(uint8_t);
	P(uint16_t);
	P(uint32_t);
	P(uint64_t);
	P(uintptr_t);
	P(size_t);
	P(void *);
	P(ptrdiff_t);
	P(off_t);
	P(_Vx_ticks_t);
	P(_Vx_ticks64_t);
	P(_Vx_freq_t);
	P(_Vx_usr_arg_t);
	P(SEM_ID);
	P(MSG_Q_ID);
	P(TASK_ID);
	P(SD_ID);
	P(RTP_ID);
	P(time_t);
	P(struct timespec);
	P(struct tm);
	P(struct statfs64);
	P(struct stat);
	P(REG_SET);
	P(ISR_DEFER_QUEUE_ID);
	P(ISR_DEFER_JOB);
	P(struct utsname);
	P(u_char);
	P(u_short);
	P(u_int);
	P(u_long);
}
