.section .text
.globl _start

_start:
	addi x1, x0, 48
	ld x2, 0(x1)       # low word is 0x80000000
	ld x3, 8(x1)       # low word is 1
	subw x4, x2, x3    # 0x80000000 - 1 wraps to positive 0x7fffffff
	subw x5, x3, x2    # 1 - 0x80000000 -> 0xffffffff80000001
	addi x6, x0, -1
	subw x7, x0, x6    # 0 - (-1) = 1
	subw x8, x6, x3    # -1 - 1 = -2
	subw x9, x2, x2    # equal operands produce zero
	subw x10, x2, x0   # upper half of rs1 is ignored
	subw x0, x2, x3    # writes targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0x00, 0x00, 0x00, 0x80, 0x78, 0x56, 0x34, 0x12
	.byte 0x01, 0x00, 0x00, 0x00, 0x21, 0x43, 0x65, 0x87
end:
