.section .text
.globl _start

_start:
	xori x1, x0, 0x1  # x1 = 0x1
	xori x1, x1, 0xf  # x1 = 0xe
	xori x1, x1, 0x1  # x1 = 0xf
	xori x1, x1, -0x1 # x1 = 0xfffffff0

