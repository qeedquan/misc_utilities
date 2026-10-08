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

#define P(x) printf("%-32s: %zu bytes\n", #x, sizeof(x));

int
main(int argc, char *argv[])
{
	P(BOOL);
	P(STATUS);
	P(VIRT_ADDR);
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

	return 0;
}
