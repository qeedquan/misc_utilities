// Example to register custom system calls

#include <vxWorks.h>
#include <syscallLib.h>
#include <syscall.h>
#include <stdio.h>
#include <ioLib.h>
#include <private/rtpLibP.h>
#include <fcntl.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

// Structures define how userspace will pass the arguments in through syscall
typedef struct {
	const char *name;
} Person;

typedef struct {
	_Vx_usr_arg_t arg0;
	_Vx_usr_arg_t arg1;
	_Vx_usr_arg_t arg2;
} Arg;

int
syshello(Person *p)
{
	printf("%s: %d: Hello, %s!\n", __func__, __LINE__, p->name);
	return OK;
}

int
sysnop(void)
{
	printf("%s: %d\n", __func__, __LINE__);
	return OK;
}

int
sysarg(Arg *a)
{
	printf("%s: %d: %ld %ld %ld\n", __func__, __LINE__, (long)a->arg0, (long)a->arg1, (long)a->arg2);
	return a->arg0 + a->arg1 + a->arg2;
}

_WRS_DATA_ALIGN_BYTES(16)
SYSCALL_RTN_TBL_ENTRY custom_syscalls[] = {
    SYSCALL_DESC_ENTRY(syshello, "syshello", 1),
    SYSCALL_DESC_ENTRY(sysnop, "sysnop", 0),
    SYSCALL_DESC_ENTRY(sysarg, "sysarg", 3),
};

int
install_custom_syscall(void)
{
	return syscallGroupRegister(SCG_USER1, "custom_syscall", nelem(custom_syscalls), custom_syscalls, 0);
}
