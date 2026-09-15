;; long-nop instructions: nopX inserts a nop of X bytes
;; see "Table 4-12. Recommended Multi-Byte Sequence of NOP Instruction" in
;; "Intel® 64 and IA-32 Architectures Software Developer’s Manual" (325383-061US)
%define nop1 nop                                                     ; just a nop, included for completeness
%define nop2 db 0x66, 0x90                                           ; 66 NOP
%define nop3 db 0x0F, 0x1F, 0x00                                     ;    NOP DWORD ptr [EAX]
%define nop4 db 0x0F, 0x1F, 0x40, 0x00                               ;    NOP DWORD ptr [EAX + 00H]
%define nop5 db 0x0F, 0x1F, 0x44, 0x00, 0x00                         ;    NOP DWORD ptr [EAX + EAX*1 + 00H]
%define nop6 db 0x66, 0x0F, 0x1F, 0x44, 0x00, 0x00                   ; 66 NOP DWORD ptr [EAX + EAX*1 + 00H]
%define nop7 db 0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00, 0x00             ;    NOP DWORD ptr [EAX + 00000000H]
%define nop8 db 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00       ;    NOP DWORD ptr [EAX + EAX*1 + 00000000H]
%define nop9 db 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00 ; 66 NOP DWORD ptr [EAX + EAX*1 + 00000000H]

global start

section .text

start:
	nop1
	nop2
	nop3
	nop4
	nop5
	nop6
	nop7
	nop8
	nop9
