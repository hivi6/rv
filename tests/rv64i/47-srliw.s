.section .text
.globl _start

_start:
	addi x1, x0, 40
	ld x2, 0(x1)       # low word is zero; upper bits must not shift into it
	srliw x3, x2, 1    # result must remain zero
	ld x4, 8(x1)       # low word is 0x80000000
	srliw x5, x4, 1    # logical word shift produces 0x40000000
	srliw x6, x4, 31   # highest legal W shift amount
	srliw x7, x4, 0    # word result is still sign-extended to XLEN
	srliw x0, x4, 1    # writes targeting x0 must be discarded
	addi x8, x0, 0     # keep embedded data naturally aligned
	jal x0, end        # skip over the embedded data

data:
	.byte 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff
	.byte 0x00, 0x00, 0x00, 0x80, 0x78, 0x56, 0x34, 0x12
end:
