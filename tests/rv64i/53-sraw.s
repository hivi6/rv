.section .text
.globl _start

_start:
	addi x1, x0, 56
	ld x2, 0(x1)       # bit 31 set while bit 63 is clear
	addi x3, x0, 1
	sraw x4, x2, x3    # arithmetic shift uses the word sign bit
	addi x5, x0, 31
	sraw x6, x2, x5    # negative word shifts to -1
	addi x7, x0, 32
	sraw x8, x2, x7    # low five shift bits select shift zero
	addi x9, x0, 33
	sraw x10, x2, x9   # low five shift bits select shift one
	ld x11, 8(x1)      # bit 31 clear while bit 63 is set
	sraw x12, x11, x3  # upper half must not control the sign
	sraw x0, x2, x3    # writes targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00
	.byte 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff
end:
