.section .text
.globl _start

_start:
	auipc x1, 0        # x1 = 0x00000000 (PC 0x0)
	auipc x2, 1        # x2 = 0x00001004 (PC 0x4 + 0x1000)
	auipc x3, 0xfffff  # x3 = 0xfffffffffffff008 (PC 0x8 - 0x1000)
	auipc x0, 0x12345  # writes to x0 must be ignored
