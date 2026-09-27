.section .text
.globl _start

_start:
	addi x1, x0, 32
	lwu x2, 0(x1)      # zero-extends 0x80000000
	lw x3, 0(x1)       # contrasting signed load sign-extends
	lwu x4, 4(x1)      # positive word is preserved
	addi x5, x1, 8
	lwu x6, -4(x5)     # negative immediate addresses the second word
	lwu x0, 0(x1)      # loads targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0x00, 0x00, 0x00, 0x80
	.byte 0x12, 0x34, 0x56, 0x78
end:
