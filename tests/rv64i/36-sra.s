.section .text
.globl _start

_start:
	addi x1, x0, -16
	addi x2, x0, 2
	sra x3, x1, x2      # -16 >> 2 = -4
	lui x4, 0x80000
	addi x5, x0, 1
	sra x6, x4, x5      # sign bit is replicated
	addi x7, x0, 31
	sra x8, x4, x7      # sign-extended LUI value >> 31 = -1
	addi x9, x0, 32
	sra x10, x4, x9     # RV64 uses six bits: shift by 32
	addi x11, x0, 33
	sra x12, x4, x11    # RV64 uses six bits: shift by 33
	addi x13, x0, -1
	sra x14, x4, x13    # low 6 bits of -1 select shift count 63
	addi x15, x0, 16
	sra x16, x15, x2    # positive values shift in zeros
	sra x17, x0, x2     # x0 can be used as a source
	sra x0, x4, x5      # writes targeting x0 must be discarded
