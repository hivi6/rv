.section .text
.globl _start

_start:
	addi x1, x0, 40
	ld x2, 0(x1)       # upper word must not affect the W result
	addiw x3, x2, 1    # 0x7fffffff + 1 -> 0xffffffff80000000
	addiw x4, x2, -1   # 0x7fffffff - 1 -> 0x000000007ffffffe
	ld x5, 8(x1)
	addiw x6, x5, -1   # 0x80000000 - 1 wraps to positive 0x7fffffff
	addiw x7, x0, -1   # negative result is sign-extended to XLEN
	addiw x0, x2, 1    # writes targeting x0 must be discarded
	addi x8, x0, 0     # keep embedded data naturally aligned
	jal x0, end        # skip over the embedded data

data:
	.byte 0xff, 0xff, 0xff, 0x7f, 0x78, 0x56, 0x34, 0x12
	.byte 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00
end:
