.section .text
.globl _start

_start:
	addi x1, x0, 0xff   # x1 = 0x000000ff
	sltiu x2, x1, 0x1ff # x2 = 0x00000001
	sltiu x2, x1, 0x1   # x2 = 0x00000000

