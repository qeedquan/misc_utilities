#include <vxWorks.h>
#include <stdio.h>
#include <cpuset.h>
#include <vxCpuLib.h>

void
show(void)
{
	unsigned int i, n;
	cpuset_t s;

	n = vxCpuConfiguredGet();
	s = vxCpuEnabledGet();
	
	printf("Number of CPUs: %u\n", n);
	for (i = 0; i < n; i++) {
		if (CPUSET_ISSET(s, i))
			printf("CPU %u is up\n", i);
	}
}

int
main(int argc, char *argv[])
{
	show();
	return 0;
}
