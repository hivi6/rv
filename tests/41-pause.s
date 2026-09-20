.section .text
.globl _start

_start:
	addi x1, x0, 30
	.word 0x0100000f       # pause
	addi x2, x1, 3

