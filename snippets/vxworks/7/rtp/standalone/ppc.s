// Minimallly, we need a text and data segment, and some symbols that the linker scripts expects

.text
.align 2
.global _start
.global __fini

addi 0, 0, 0
_start:
	b _start

__fini:

.type   _start,@function
.size   _start,.-_start

.data
.align 2
.byte 0x55
