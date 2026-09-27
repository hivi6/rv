.section .text
.globl _start

_start:
	addi x1, x0, 56
	ld x2, 0(x1)       # source value: 0x123456789abcdef0
	sd x2, 8(x1)       # store all 64 bits
	ld x3, 8(x1)       # reload the stored doubleword
	lbu x4, 8(x1)      # least-significant byte is stored first
	lbu x5, 15(x1)     # most-significant byte is stored last
	addi x6, x1, 24
	sd x2, -8(x6)      # negative immediate stores at data + 16
	ld x7, 16(x1)
	ld x8, 24(x1)      # neighboring doubleword remains unchanged
	sd x0, 24(x1)      # x0 can be used as the source
	ld x9, 24(x1)
	addi x10, x0, 0    # keep embedded data naturally aligned
	jal x0, end        # skip over the embedded data

data:
	.byte 0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12
	.byte 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x10, 0x20
	.byte 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
	.byte 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
end:
