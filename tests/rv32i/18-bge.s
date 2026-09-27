.section .text
.globl _start

_start:
	addi x1, x0, -1
	addi x2, x0, 1
	bge x2, x1, positive_ge  # signed 1 >= -1; taken
	addi x3, x0, 1           # skipped
positive_ge:
	addi x4, x0, 1
	bge x1, x2, wrong_order  # signed -1 >= 1; not taken
	addi x5, x0, 1
wrong_order:
	bge x2, x2, equal        # equal values; taken
	addi x6, x0, 1           # skipped
equal:
	addi x7, x0, 1

	addi x8, x0, 2
	addi x9, x0, 1
	bge x8, x9, check        # forward branch taken
body:
	addi x8, x0, 0
check:
	bge x8, x9, body         # backward branch taken once, then not taken
	addi x10, x0, 1
