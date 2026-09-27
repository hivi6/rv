.section .text
.globl _start

_start:
	addi x1, x0, -1  # x1 = 0xffffffffffffffff
	srli x1, x1, 0x1 # x1 = 0x7fffffffffffffff
	srli x1, x1, 0x1 # x1 = 0x3fffffffffffffff
	srli x1, x1, 0x1 # x1 = 0x1fffffffffffffff
	srli x1, x1, 0x1 # x1 = 0x0fffffffffffffff
	addi x2, x0, -1
	srli x3, x2, 32  # x3 = 0x00000000ffffffff
	srli x4, x2, 63  # x4 = 1
