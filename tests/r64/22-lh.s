.section .text
.globl _start

_start:
	addi x1, x0, 32    # After 32 bytes we have data
	lh x2, 0(x1)       # 0x7fff is positive
	lh x3, 2(x1)       # 0x8000 sign-extends to 0xffffffffffff8000
	lh x4, 4(x1)       # 0xffff sign-extends to 0xffffffffffffffff
	addi x5, x1, 4
	lh x6, -2(x5)      # negative immediate addresses the 0x8000 halfword
	lh x0, 0(x1)       # loads targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0xff, 0x7f
	.byte 0x00, 0x80
	.byte 0xff, 0xff
	.balign 4
end:
