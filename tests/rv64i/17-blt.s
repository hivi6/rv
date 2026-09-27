.section .text
.globl _start

_start:
	addi x1, x0, -1
	addi x2, x0, 1
	blt x1, x2, negative_less  # signed -1 < 1; taken
	addi x3, x0, 1             # skipped
negative_less:
	addi x4, x0, 1
	blt x2, x1, wrong_order    # signed 1 < -1; not taken
	addi x5, x0, 1
wrong_order:

	addi x6, x0, 0
	addi x7, x0, 1
	blt x6, x7, check          # forward branch taken
body:
	addi x6, x0, 2
check:
	blt x6, x7, body           # backward branch taken once, then not taken
	addi x8, x0, 1
	blt x7, x7, end            # equal values; not taken
	addi x9, x0, 1
end:
