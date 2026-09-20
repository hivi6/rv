.section .text
.globl _start

_start:
	addi x1, x0, -1     # x1 = 0xffffffff
	andi x1, x1, 0x1ff  # x1 = 0x000001ff
	andi x1, x1, 0x11f  # x1 = 0x0000011f
	andi x1, x1, 0x1ff  # x1 = 0x0000011f
	andi x1, x1, 0x00f  # x1 = 0x0000000f

