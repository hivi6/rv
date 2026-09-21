.section .text
.globl _start

_start:
	addi x1, x0, 48
	ld x2, 0(x1)       # bit 31 set while bit 63 is clear
	sraiw x3, x2, 1    # arithmetic shift uses the word sign bit
	sraiw x4, x2, 31   # negative word shifts to -1
	ld x5, 8(x1)       # bit 31 clear while bit 63 is set
	sraiw x6, x5, 1    # upper 32 bits must be ignored
	sraiw x7, x5, 31   # positive word shifts to zero
	sraiw x8, x2, 0    # shamt zero still sign-extends the word
	sraiw x9, x2, 4
	sraiw x10, x5, 4
	sraiw x0, x2, 1    # writes targeting x0 must be discarded
	jal x0, end         # skip over the embedded data

data:
	.byte 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00
	.byte 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff
end:
