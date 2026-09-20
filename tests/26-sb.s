.section .text
.globl _start

_start:
	addi x1, x0, 44
	addi x2, x0, -1
	sb x2, 0(x1)       # store only the low byte: 0xff
	lbu x3, 0(x1)      # reload as 0x000000ff
	lb x4, 0(x1)       # reload as 0xffffffff
	addi x2, x0, 127
	addi x5, x1, 2
	sb x2, -1(x5)      # negative immediate stores 0x7f at data + 1
	lbu x6, 1(x1)
	lw x7, 0(x1)       # neighboring bytes remain unchanged
	jal x0, end        # skip over the embedded data

data:
	.byte 0x12, 0x34, 0x56, 0x78
end:
