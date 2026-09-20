.section .text
.globl _start

_start:
	lui x1, 0x55555
	addi x1, x1, 0x555 # x1 = 0x55555555
	lui x2, 0x33333
	addi x2, x2, 0x333 # x2 = 0x33333333
	or x3, x1, x2      # overlapping patterns produce 0x77777777
	or x4, x1, x1      # a value OR itself is unchanged
	or x5, x1, x0      # a value OR zero is unchanged
	addi x6, x0, -1
	or x7, x1, x6      # OR with all ones produces all ones
	lui x8, 0x80000
	addi x9, x0, 1
	or x10, x8, x9     # combine the high and low bits
	or x1, x1, x2      # destination may alias a source register
	or x0, x1, x2      # writes targeting x0 must be discarded

