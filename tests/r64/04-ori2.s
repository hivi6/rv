.section .text
.globl _start

_start:
	ori x1, x1, -256 # x1 = 0xffffffffffffff00
	ori x1, x1, 0xf  # x1 = 0xffffffffffffff0f
	ori x1, x1, 0xf0 # x1 = 0xffffffffffffffff
