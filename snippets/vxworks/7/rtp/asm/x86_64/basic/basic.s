#include <stdio.h>

.globl add
.globl hello
.globl return87

.text
add:
	add %rsi, %rdi
	mov %rdi, %rax
	ret

hello:
	mov $message, %rdi
	call puts
	ret

return87:
	mov $87, %rax
	ret

message:
	.ascii "Hello\x00"
