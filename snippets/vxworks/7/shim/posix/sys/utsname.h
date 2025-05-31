#ifndef _UTSNAME_H_
#define _UTSNAME_H_

#define _UTSNAME_SHORT_NAME_SIZE 80
#define _UTSNAME_LONG_NAME_SIZE 256
#define _UTSNAME_NUMBER_SIZE 8

#define utsname vxutsname
#define uname vxuname

struct vxutsname {
	char sysname[_UTSNAME_SHORT_NAME_SIZE];        /* operating system name */
	char nodename[_UTSNAME_LONG_NAME_SIZE];        /* network node name */
	char release[_UTSNAME_SHORT_NAME_SIZE];        /* OS release level */
	char version[_UTSNAME_LONG_NAME_SIZE];         /* operating system version */
	char machine[_UTSNAME_LONG_NAME_SIZE];         /* hardware type (BSP model) */
	char endian[_UTSNAME_SHORT_NAME_SIZE];         /* architecture endianness */
	char kernelversion[_UTSNAME_SHORT_NAME_SIZE];  /* VxWorks kernel version */
	char releaseversion[_UTSNAME_SHORT_NAME_SIZE]; /* release version */
	char processor[_UTSNAME_SHORT_NAME_SIZE];      /* CPU type */
	char bsprevision[_UTSNAME_SHORT_NAME_SIZE];    /* VxWorks BSP revision */
	char builddate[_UTSNAME_SHORT_NAME_SIZE];      /* VxWorks kernel build date */
};

int vxuname(struct utsname *buf);

#endif
