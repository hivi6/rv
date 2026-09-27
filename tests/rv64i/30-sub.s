.section .text
.globl _start

_start:
	addi x1, x0, 20
	addi x2, x0, 7
	sub x3, x1, x2      # operand order: 20 - 7 = 13
	sub x4, x2, x1      # 7 - 20 = -13
	sub x5, x1, x1      # equal operands produce zero
	sub x6, x0, x2      # 0 - 7 = -7
	sub x7, x2, x0      # subtracting zero preserves the value
	addi x8, x0, -1
	sub x9, x0, x8      # XLEN wraparound: 0 - RV64 max = 1
	lui x10, 0x80000
	addi x11, x0, 1
	sub x12, x10, x11   # sign-extended LUI value minus one
	sub x0, x1, x2      # writes targeting x0 must be discarded
