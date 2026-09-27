.section .text
.globl _start

_start:
	addi x1, x0, 32
	lw x2, 0(x1)       # positive word: 0x78563412
	lw x3, 4(x1)       # sign bit set: 0x80000000
	lw x4, 8(x1)       # all bits set: 0xffffffff
	addi x5, x1, 8
	lw x6, -4(x5)      # negative immediate addresses 0x80000000
	lw x0, 0(x1)       # loads targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0x12, 0x34, 0x56, 0x78
	.byte 0x00, 0x00, 0x00, 0x80
	.byte 0xff, 0xff, 0xff, 0xff
end:
