.section .text
.globl _start

_start:
	addi x1, x0, 1
	addi x2, x0, 1
	beq x1, x2, check    # forward branch taken; skip body initially
body:
	addi x1, x0, 2
check:
	beq x1, x2, body     # backward branch taken once, then not taken
	addi x3, x0, 1
	beq x1, x1, forward  # forward branch taken
	addi x4, x0, 1       # skipped
forward:
	addi x5, x0, 1
	beq x1, x2, end      # not taken
	addi x6, x0, 1
end:
