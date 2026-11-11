// Minimallly, we need a text and data segment, and some symbols that the linker scripts expects

.text
.align 16
.global _start
.global __fini

nop
_start:
	jmp _start

__fini:

.type   _start,@function
.size   _start,.-_start

.data
.align 16
.byte 0xde, 0xad
