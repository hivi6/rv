.section .text
.globl _start

_start:
	addi x1, x0, 48
	ld x2, 0(x1)
	ld x3, 8(x1)
	addw x4, x2, x3   # 0x7fffffff + 1 -> 0xffffffff80000000
	ld x5, 16(x1)
	addi x6, x0, -1
	addw x7, x5, x6   # 0x80000000 - 1 wraps to positive 0x7fffffff
	addw x8, x6, x6   # -1 + -1 -> -2
	addw x9, x0, x3   # upper half of rs2 is ignored
	addw x10, x2, x0  # upper half of rs1 is ignored
	addw x0, x2, x3   # writes targeting x0 must be discarded
	jal x0, end       # skip over the embedded data

data:
	.byte 0xff, 0xff, 0xff, 0x7f, 0x78, 0x56, 0x34, 0x12
	.byte 0x01, 0x00, 0x00, 0x00, 0x21, 0x43, 0x65, 0x87
	.byte 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00
end:
