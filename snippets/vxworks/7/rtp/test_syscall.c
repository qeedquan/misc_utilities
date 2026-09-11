#include <vxWorks.h>
#include <stdio.h>
#include <syscall.h>

int
main(int argc, char *argv[])
{
	const char name[] = "Hello\n";

	syscall(1, (_Vx_usr_arg_t)name, sizeof(name) - 1, 0, 0, 0, 0, 0, SCN__write);
	syscall(1, 0, 0, 0, 0, 0, 0, 0, SCN__exit);
	return 0;

	(void)argc;
	(void)argv;
}
