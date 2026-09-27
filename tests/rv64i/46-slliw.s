.section .text
.globl _start

_start:
	addi x1, x0, 40
	ld x2, 0(x1)       # low word is 1; upper word must be ignored
	slliw x3, x2, 31   # result bit 31 is sign-extended
	slliw x4, x2, 1
	ld x5, 8(x1)       # low word is 0x80000001
	slliw x6, x5, 1    # word overflow is discarded
	slliw x7, x5, 0    # even shamt zero sign-extends the word result
	slliw x0, x2, 1    # writes targeting x0 must be discarded
	addi x8, x0, 0     # keep embedded data naturally aligned
	jal x0, end        # skip over the embedded data

data:
	.byte 0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
	.byte 0x01, 0x00, 0x00, 0x80, 0x78, 0x56, 0x34, 0x12
end:
