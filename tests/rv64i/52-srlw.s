.section .text
.globl _start

_start:
	addi x1, x0, 56
	ld x2, 0(x1)       # low word is 0x80000000
	addi x3, x0, 1
	srlw x4, x2, x3    # logical word shift produces 0x40000000
	addi x5, x0, 31
	srlw x6, x2, x5    # highest word shift produces 1
	addi x7, x0, 32
	srlw x8, x2, x7    # low five shift bits select shift zero
	addi x9, x0, 33
	srlw x10, x2, x9   # low five shift bits select shift one
	ld x11, 8(x1)      # low word is zero; upper half must be ignored
	srlw x12, x11, x3
	srlw x0, x2, x3    # writes targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff
	.byte 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
end:
