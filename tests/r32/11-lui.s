.section .text
.globl _start

_start:
	lui x1, 0x12345  # x1 = 0x12345000
	lui x2, 0xfffff  # x2 = 0xfffff000
	lui x0, 0x54321  # writes to x0 must be ignored
