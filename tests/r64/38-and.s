.section .text
.globl _start

_start:
	lui x1, 0x55555
	addi x1, x1, 0x555 # x1 = 0x55555555
	lui x2, 0x33333
	addi x2, x2, 0x333 # x2 = 0x33333333
	and x3, x1, x2     # overlapping patterns produce 0x11111111
	and x4, x1, x1     # a value AND itself is unchanged
	and x5, x1, x0     # a value AND zero is zero
	addi x6, x0, -1
	and x7, x1, x6     # AND with all ones is unchanged
	lui x8, 0x80000
	addi x9, x0, 1
	and x10, x8, x9    # disjoint high and low bits produce zero
	and x1, x1, x2     # destination may alias a source register
	and x0, x1, x2     # writes targeting x0 must be discarded

