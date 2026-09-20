.section .text
.globl _start

_start:
	addi x1, x0, -1  # x1 = 0xffffffff
	srli x1, x1, 0x1 # x1 = 0x7fffffff
	srli x1, x1, 0x1 # x1 = 0x3fffffff
	srli x1, x1, 0x1 # x1 = 0x1fffffff
	srli x1, x1, 0x1 # x1 = 0x0fffffff

