// Example on how to use syscall to directly call our custom system calls
// Need the custom syscall DKM to be loaded before running this

#include <vxWorks.h>
#include <stdio.h>
#include <syscall.h>

int
main(int argc, char *argv[])
{
	const char *name = "Xegozaur";

	syscall((_Vx_usr_arg_t)name, 0, 0, 0, 0, 0, 0, 0, SYSCALL_NUMBER(SCG_USER1, 0));
	syscall(0, 0, 0, 0, 0, 0, 0, 0, SYSCALL_NUMBER(SCG_USER1, 1));
	printf("%d\n", syscall(53, 45, 13, 0, 0, 0, 0, 0, SYSCALL_NUMBER(SCG_USER1, 2)));
	return 0;

	(void) argc;
	(void) argv;
}
