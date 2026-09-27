.section .text
.globl _start

_start:
	addi x1, x0, 0x1 # x1 = 0x00000001
	slli x1, x1, 0x1 # x1 = 0x00000002
	slli x1, x1, 0x1 # x1 = 0x00000004
	slli x1, x1, 0x1 # x1 = 0x00000008
	slli x1, x1, 0x1 # x1 = 0x00000010
	slli x1, x1, 0x1 # x1 = 0x00000020
	slli x1, x1, 0x2 # x1 = 0x00000080

