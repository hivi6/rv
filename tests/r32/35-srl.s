.section .text
.globl _start

_start:
	addi x1, x0, -1
	addi x2, x0, 4
	srl x3, x1, x2      # 0xffffffff >> 4 = 0x0fffffff
	lui x4, 0x80000
	addi x5, x0, 1
	srl x6, x4, x5      # logical shift clears the high bit
	addi x7, x0, 31
	srl x8, x4, x7      # highest RV32 shift count
	addi x9, x0, 32
	srl x10, x4, x9     # low 5 bits select shift count zero
	addi x11, x0, 33
	srl x12, x4, x11    # low 5 bits select shift count one
	addi x13, x0, -1
	srl x14, x4, x13    # low 5 bits of -1 select shift count 31
	srl x15, x0, x2     # x0 can be used as a source
	srl x0, x4, x5      # writes targeting x0 must be discarded

