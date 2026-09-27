.section .text
.globl _start

_start:
	addi x1, x0, 32
	ld x2, 0(x1)       # x2 = 0x8877665544332211
	ld x3, 8(x1)       # x3 = 0x0123456789abcdef
	addi x4, x1, 16
	ld x5, -8(x4)      # negative immediate addresses the second doubleword
	ld x0, 0(x1)       # loads targeting x0 must be discarded
	lbu x6, 0(x1)      # confirms little-endian byte order
	jal x0, end        # skip over the embedded data

data:
	.byte 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
	.byte 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01
end:
