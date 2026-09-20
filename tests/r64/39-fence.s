.section .text
.globl _start

_start:
	addi x1, x0, 11
	fence iorw, iorw
	addi x2, x1, 1

