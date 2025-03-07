#ifndef _VMLIB_COMMON_H_
#define _VMLIB_COMMON_H_

#define MMU_ATTR_SUP_RO (MMU_ATTR_PROT_SUP_READ)
#define MMU_ATTR_SUP_RW (MMU_ATTR_PROT_SUP_READ | MMU_ATTR_PROT_SUP_WRITE)
#define MMU_ATTR_SUP_EXE (MMU_ATTR_PROT_SUP_READ | MMU_ATTR_PROT_SUP_EXE)
#define MMU_ATTR_SUP_RWX (MMU_ATTR_SUP_RW | MMU_ATTR_PROT_SUP_EXE)

#define MMU_ATTR_USR_RO (MMU_ATTR_PROT_USR_READ)
#define MMU_ATTR_USR_RW (MMU_ATTR_PROT_USR_READ | MMU_ATTR_PROT_USR_WRITE)
#define MMU_ATTR_USR_EXE (MMU_ATTR_PROT_USR_READ | MMU_ATTR_PROT_USR_EXE)
#define MMU_ATTR_USR_RWX (MMU_ATTR_USR_RW | MMU_ATTR_PROT_USR_EXE)

#define MMU_ATTR_SUP_TEXT (MMU_ATTR_SUP_EXE)
#define MMU_ATTR_SUP_TEXT_W (MMU_ATTR_SUP_RWX)
#define MMU_ATTR_SUP_DATA (MMU_ATTR_SUP_RW)
#define MMU_ATTR_SUP_DATA_RO (MMU_ATTR_SUP_RO)

#endif
