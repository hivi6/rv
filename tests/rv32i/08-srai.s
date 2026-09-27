.section .text
.globl _start

_start:
	addi x1, x0, -1   # x1 = 0xffffffff
	xori x1, x1, 0xff # x1 = 0xffffff00
	srai x1, x1, 1    # x1 = 0xffffff80
	srai x1, x1, 1    # x1 = 0xffffffc0
	srai x1, x1, 1    # x1 = 0xffffffe0
	srai x1, x1, 2    # x1 = 0xfffffff8

