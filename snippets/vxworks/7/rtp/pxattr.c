#include <stdio.h>
#include <private/taskPxLibP.h>

int
main(int argc, char *argv[])
{
	TASK_PX_ATTR px;

	if ((taskPxAttrGet(0, &px)) == ERROR)
		printf("Not supported\n");
	else
		printf("Supported\n");
	return 0;
}
