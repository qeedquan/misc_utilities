#ifndef _VX_CPULIB_H_
#define _VX_CPULIB_H_

#define CPUSET_ISSET(set, index) (1)

cpuset_t vxCpuEnabledGet(void);
unsigned int vxCpuConfiguredGet(void);

#endif
