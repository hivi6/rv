.section .text
.globl _start

_start:
	jal x1, forward  # x1 = 0x00000004; jump from PC 0x0 to 0xc
back:
	addi x2, x0, 1   # reached by the backward jump
	jal x0, end      # jump from PC 0x8 to 0x14; x0 remains zero
forward:
	jal x3, back     # x3 = 0x00000010; jump backward to PC 0x4
	addi x4, x0, 1   # skipped
end:
	addi x5, x0, 1
