.section .text
.globl _start

_start:
	addi x1, x0, -1
	slti x2, x1, 1   # -1 < 1, so x2 = 1
	slti x3, x1, -2  # -1 < -2 is false, so x3 = 0
	addi x4, x0, 5
	slti x5, x4, 10  # 5 < 10, so x5 = 1
	slti x6, x4, -1  # 5 < -1 is false, so x6 = 0

