.section .text
.globl _start

_start:
	addi x1, x0, -1
	addi x2, x0, 1
	bgeu x1, x2, unsigned_ge  # unsigned max >= 1; taken
	addi x3, x0, 1            # skipped
unsigned_ge:
	addi x4, x0, 1
	bgeu x2, x1, wrong_order  # unsigned 1 >= max; not taken
	addi x5, x0, 1
wrong_order:
	bgeu x2, x2, equal        # equal values; taken
	addi x6, x0, 1            # skipped
equal:
	addi x7, x0, 1

	addi x8, x0, 2
	addi x9, x0, 1
	bgeu x8, x9, check        # forward branch taken
body:
	addi x8, x0, 0
check:
	bgeu x8, x9, body         # backward branch taken once, then not taken
	addi x10, x0, 1
