.section .text
.globl _start

_start:
	addi x1, x0, 52
	lui x2, 0x12345
	addi x2, x2, 0x678
	sw x2, 0(x1)       # store 0x12345678
	lw x3, 0(x1)
	lbu x4, 0(x1)      # least-significant byte is stored first
	lbu x5, 3(x1)      # most-significant byte is stored last
	addi x2, x0, -1
	addi x6, x1, 8
	sw x2, -4(x6)      # negative immediate stores at data + 4
	lw x7, 4(x1)
	lw x8, 8(x1)       # neighboring word remains unchanged
	jal x0, end        # skip over the embedded data

data:
	.byte 0xaa, 0xbb, 0xcc, 0xdd
	.byte 0x10, 0x20, 0x30, 0x40
	.byte 0x11, 0x22, 0x33, 0x44
end:
