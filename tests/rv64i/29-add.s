.section .text
.globl _start

_start:
	addi x1, x0, 10
	addi x2, x0, 20
	add x3, x1, x2      # 10 + 20 = 30
	addi x4, x0, -5
	add x5, x3, x4      # 30 + (-5) = 25
	addi x6, x0, -1
	addi x7, x0, 1
	add x8, x6, x7      # XLEN wraparound: RV64 max + 1 = 0
	lui x9, 0x80000
	add x10, x9, x9     # sign-extended LUI values add at full XLEN
	add x11, x0, x1     # x0 can be used as a source
	add x12, x6, x6     # -1 + -1 = 0xfffffffffffffffe
	add x0, x1, x2      # writes targeting x0 must be discarded
