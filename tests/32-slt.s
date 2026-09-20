.section .text
.globl _start

_start:
	addi x1, x0, -1
	addi x2, x0, 1
	slt x3, x1, x2      # -1 < 1
	slt x4, x2, x1      # 1 is not less than -1
	slt x5, x1, x1      # equal values are not less than
	addi x6, x0, -2
	slt x7, x6, x1      # -2 < -1
	slt x8, x1, x6      # -1 is not less than -2
	lui x9, 0x80000     # signed minimum: 0x80000000
	lui x10, 0x80000
	addi x10, x10, -1   # signed maximum: 0x7fffffff
	slt x11, x9, x10    # signed minimum < signed maximum
	slt x12, x10, x9
	slt x13, x0, x2     # zero < one
	slt x14, x1, x0     # negative one < zero
	slt x0, x1, x2      # writes targeting x0 must be discarded

