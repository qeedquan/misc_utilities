#ifndef _MMULIB_H_
#define _MMULIB_H_

#define VX_MEM_BARRIER_RW()

enum {
	MMU_ATTR_PROT_SUP_READ = 1 << 0,
	MMU_ATTR_VALID = 1 << 1,
};

#endif
