#include "vxWorks.h"

cpuset_t
vxCpuEnabledGet(void)
{
	cpuset_t s = {0};
	return s;
}

unsigned int
vxCpuConfiguredGet(void)
{
	return 1;
}
