.section .text
.globl _start

_start:
	addi x1, x0, 32    # After 32 bytes we have data
	lbu x2, 0(x1)      # 0x7f zero-extends to 0x0000007f
	lbu x3, 1(x1)      # 0x80 zero-extends to 0x00000080
	lbu x4, 2(x1)      # 0xff zero-extends to 0x000000ff
	addi x5, x1, 2
	lbu x6, -1(x5)     # negative immediate addresses the 0x80 byte
	lbu x0, 0(x1)      # loads targeting x0 must be discarded
	jal x0, end        # skip over the embedded data

data:
	.byte 0x7f, 0x80, 0xff, 0x12
end:
