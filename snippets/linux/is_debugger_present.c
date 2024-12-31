#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool
isdebugged(void)
{
	FILE *fp;
	char buf[4096];
	bool ret;

	fp = fopen("/proc/self/status", "r");
	if (!fp)
		return false;

	ret = true;
	while (fgets(buf, sizeof(buf), fp)) {
		if (strcmp(buf, "TracerPid:\t0\n") == 0) {
			ret = false;
			break;
		}
	}
	fclose(fp);
	return ret;
}

int
main(void)
{
	printf("%d\n", isdebugged());
	return 0;
}
