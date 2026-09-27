.section .text
.globl _start

_start:
	addi x1, x0, 20
	.word 0x8330000f       # fence.tso
	addi x2, x1, 2
