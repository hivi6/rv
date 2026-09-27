.section .text
.globl _start

_start:
	addi x1, x0, -1
	addi x2, x0, 1
	bltu x1, x2, wrong_order    # RV64 unsigned max < 1; not taken
	addi x3, x0, 1
wrong_order:
	bltu x2, x1, unsigned_less  # unsigned 1 < RV64 max; taken
	addi x4, x0, 1              # skipped
unsigned_less:
	addi x5, x0, 1
	bltu x2, x2, equal          # equal values; not taken
	addi x6, x0, 1
equal:

	addi x7, x0, 0
	addi x8, x0, 1
	bltu x7, x8, check          # forward branch taken
body:
	addi x7, x0, 2
check:
	bltu x7, x8, body           # backward branch taken once, then not taken
	addi x9, x0, 1
