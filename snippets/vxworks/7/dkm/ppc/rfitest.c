#include <vxWorks.h>
#include <stdio.h>
#include <unistd.h>
#include <arch/ppc/vxPpcLib.h>

void
rfihandler(void)
{
	unsigned a, b, c;

	a = b = c = 0;
	for (;;) {
		a += b + c;
		c ^= a ^ b;
		b |= a | c;
		a &= ~(b ^ c);
	}
}

/*

Return from Interrupt (RFI) is usually used to implement user processes:

NEXT_PC = SRR0[0]
MSR = SRR[1] & RFI_SAVE_MASK
RFI JUMPS TO NEXT_PC

If the environment is not setup properly before executing RFI, anything that tries to do syscall or accessing memory/stack will likely to crash and reset the OS

*/

void
rfitest(unsigned int addr, unsigned int flags)
{
	_WRS_ASM("mtsrr0 %r3");
	_WRS_ASM("mtsrr1 %r4");
	_WRS_ASM("rfi");
}
