.section .text
.globl _start

_start:
	addi x1, x0, 1
	addi x2, x0, 4
	sll x3, x1, x2      # 1 << 4 = 16
	addi x4, x0, -1
	sll x5, x4, x2      # logical shift with a full-width source
	addi x6, x0, 31
	sll x7, x1, x6      # shift by 31
	addi x8, x0, 32
	sll x9, x1, x8      # RV64 uses six bits: shift by 32
	addi x10, x0, 33
	sll x11, x1, x10    # RV64 uses six bits: shift by 33
	sll x12, x0, x2     # x0 can be used as a source
	addi x13, x0, -1
	sll x14, x1, x13    # low 6 bits of -1 select shift count 63
	sll x0, x1, x2      # writes targeting x0 must be discarded
