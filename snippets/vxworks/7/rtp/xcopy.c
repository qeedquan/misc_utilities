#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <usrFsLib.h>

int
main(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "usage: <file1> <file2>\n");
		exit(2);
	}

	if (xcopy(argv[1], argv[2]) == ERROR)
		printf("Failed to copy: %s\n", strerror(errno));

	return 0;
}
