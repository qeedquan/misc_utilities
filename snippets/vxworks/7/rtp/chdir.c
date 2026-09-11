#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void
pwd(void)
{
	char dir[256];

	if (!getcwd(dir, sizeof(dir))) {
		printf("Failed to get directory: %s\n", strerror(errno));
		return;
	}
	printf("Current directory: %s\n", dir);
}

void
newdir(const char *dir)
{
	if (chdir(dir) < 0)
		printf("Failed to change directory: %s\n", strerror(errno));
}

int
main(int argc, char *argv[])
{
	pwd();
	if (argc >= 2) {
		newdir(argv[1]);
		pwd();
	}
	return 0;
}
