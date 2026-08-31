.section .text
.globl _start

_start:
    addi x1, x0, 5      # x1 = 5
    addi x2, x1, -1     # x2 = 4
    addi x0, x0, 123    # x0 must remain 0

