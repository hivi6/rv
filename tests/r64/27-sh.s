.section .text
.globl _start

_start:
	addi x1, x0, 52
	lui x2, 0x12345
	addi x2, x2, 0x678
	sh x2, 0(x1)       # store only the low halfword: 0x5678
	lhu x3, 0(x1)
	lw x4, 0(x1)       # upper neighboring bytes remain 0x7856
	addi x2, x0, -1
	addi x5, x1, 6
	sh x2, -2(x5)      # negative immediate stores 0xffff at data + 4
	lhu x6, 4(x1)      # reload as 0x0000ffff
	lh x7, 4(x1)       # reload as 0xffffffff
	lw x8, 4(x1)       # upper neighboring bytes remain 0xf0de
	jal x0, end        # skip over the embedded data

data:
	.byte 0x12, 0x34, 0x56, 0x78
	.byte 0x9a, 0xbc, 0xde, 0xf0
end:
