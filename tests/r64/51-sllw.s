.section .text
.globl _start

_start:
	addi x1, x0, 56
	ld x2, 0(x1)       # low word is 1; upper half must be ignored
	addi x3, x0, 31
	sllw x4, x2, x3    # result bit 31 is sign-extended
	addi x5, x0, 32
	sllw x6, x2, x5    # low five shift bits select shift zero
	addi x7, x0, 33
	sllw x8, x2, x7    # low five shift bits select shift one
	ld x9, 8(x1)       # low word is 0x80000001
	addi x10, x0, 1
	sllw x11, x9, x10  # bits shifted beyond the word are discarded
	sllw x0, x2, x3    # writes targeting x0 must be discarded
	sllw x12, x9, x0   # x0 selects shift zero; result is sign-extended
	jal x0, end        # skip over the embedded data

data:
	.byte 0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
	.byte 0x01, 0x00, 0x00, 0x80, 0x78, 0x56, 0x34, 0x12
end:
