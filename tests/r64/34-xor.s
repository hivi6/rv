.section .text
.globl _start

_start:
	lui x1, 0x55555
	addi x1, x1, 0x555 # x1 = 0x55555555
	lui x2, 0x33333
	addi x2, x2, 0x333 # x2 = 0x33333333
	xor x3, x1, x2     # alternating patterns produce 0x66666666
	xor x4, x1, x1     # a value XOR itself is zero
	xor x5, x1, x0     # a value XOR zero is unchanged
	addi x6, x0, -1
	xor x7, x1, x6     # XOR with all ones complements every bit
	xor x8, x6, x6
	xor x1, x1, x2     # destination may alias a source register
	xor x0, x1, x2     # writes targeting x0 must be discarded

