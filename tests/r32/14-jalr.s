.section .text
.globl _start

_start:
	addi x1, x0, 13
	jalr x2, 0(x1)    # target = 13 & ~1 = 12; x2 = 8
	addi x3, x0, 1    # skipped
	addi x4, x0, 1

	addi x5, x0, 29
	jalr x5, 0(x5)    # target uses old x5: 29 & ~1 = 28; x5 = 24
	addi x6, x0, 1    # skipped
	addi x7, x0, 1

	addi x9, x0, 49
	jalr x8, -4(x9)   # target = (49 - 4) & ~1 = 44; x8 = 40
	addi x10, x0, 1   # skipped
	addi x11, x0, 1

	addi x12, x0, 61
	jalr x0, 0(x12)   # target = 61 & ~1 = 60; x0 remains zero
	addi x13, x0, 1   # skipped
	addi x14, x0, 1
