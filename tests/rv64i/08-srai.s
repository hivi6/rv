.section .text
.globl _start

_start:
	addi x1, x0, -1   # x1 = 0xffffffffffffffff
	xori x1, x1, 0xff # x1 = 0xffffffffffffff00
	srai x1, x1, 1    # x1 = 0xffffffffffffff80
	srai x1, x1, 1    # x1 = 0xffffffffffffffc0
	srai x1, x1, 1    # x1 = 0xffffffffffffffe0
	srai x1, x1, 2    # x1 = 0xfffffffffffffff8
	addi x2, x0, -1
	srai x3, x2, 32  # x3 remains -1
	addi x4, x0, 1
	slli x4, x4, 63  # x4 = RV64 signed minimum
	srai x5, x4, 63  # x5 = -1
