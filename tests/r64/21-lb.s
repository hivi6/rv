.section .text
.globl _start

_start:
	addi x1, x0, 32    # after 32 bytes of instruction, we have data
	lb x2, 0(x1)       # 0x7f is positive
	lb x3, 1(x1)       # 0x80 sign-extends to 0xffffffffffffff80
	lb x4, 2(x1)       # 0xff sign-extends to 0xffffffffffffffff
	addi x5, x1, 2
	lb x6, -1(x5)      # negative immediate addresses the 0x80 byte
	lb x0, 0(x1)       # loads targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0x7f, 0x80, 0xff
	.balign 4
end:
