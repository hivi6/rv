.section .text
.globl _start

_start:
	addi x1, x0, -1     # unsigned maximum: 0xffffffff
	addi x2, x0, 1
	sltu x3, x1, x2     # unsigned maximum is not less than 1
	sltu x4, x2, x1     # 1 < unsigned maximum
	sltu x5, x1, x1     # equal values are not less than
	addi x6, x0, 2
	sltu x7, x2, x6     # 1 < 2
	sltu x8, x6, x2     # 2 is not less than 1
	sltu x9, x0, x2     # 0 < 1
	sltu x10, x1, x0    # unsigned maximum is not less than zero
	lui x11, 0x80000    # 0x80000000
	sltu x12, x11, x1   # high-bit value < unsigned maximum
	sltu x13, x1, x11
	sltu x0, x2, x1     # writes targeting x0 must be discarded
