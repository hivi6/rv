.section .text
.globl _start

_start:
	addi x1, x0, 0x1 # x1 = 0x00000001
	slli x1, x1, 0x1 # x1 = 0x00000002
	slli x1, x1, 0x1 # x1 = 0x00000004
	slli x1, x1, 0x1 # x1 = 0x00000008
	slli x1, x1, 0x1 # x1 = 0x00000010
	slli x1, x1, 0x1 # x1 = 0x00000020
	slli x1, x1, 0x2 # x1 = 0x00000080
	addi x2, x0, 1
	slli x3, x2, 32  # RV64 immediate shift uses shamt[5]
	slli x4, x2, 63  # highest RV64 immediate shift amount
