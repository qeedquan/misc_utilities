#include <vxWorks.h>
#include <stdio.h>
#include <intLib.h>
#include <excLib.h>
#include <sysLib.h>

void
locations(void)
{
	printf("Exception Vector Base Address %p\n", excVecBaseGet());
	printf("Vector Base Address           %p\n", intVecBaseGet());
	printf("Sys Boot Line                 %p\n", sysBootLine);
}
