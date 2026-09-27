.section .text
.globl _start

_start:
	addi x1, x0, 32    # After 32 bytes we have the data
	lhu x2, 0(x1)      # 0x7fff zero-extends to 0x00007fff
	lhu x3, 2(x1)      # 0x8000 zero-extends to 0x00008000
	lhu x4, 4(x1)      # 0xffff zero-extends to 0x0000ffff
	addi x5, x1, 4
	lhu x6, -2(x5)     # negative immediate addresses the 0x8000 halfword
	lhu x0, 0(x1)      # loads targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0xff, 0x7f
	.byte 0x00, 0x80
	.byte 0xff, 0xff
	.balign 4
end:
