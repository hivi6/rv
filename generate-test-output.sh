#!/bin/bash

for test in `find tests/r32 -name '*.s' | sort`; do
	riscv64-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -Wl,-e,_start -Wl,-Ttext=0x0 -o $test.elf $test
	riscv64-elf-objcopy -O binary $test.elf $test.bin
	./build/rv-test32 $test.bin > $test.out
done

for test in `find tests/r64 -name '*.s' | sort`; do
	riscv64-elf-gcc -march=rv64i -mabi=lp64 -nostdlib -Wl,-e,_start -Wl,-Ttext=0x0 -o $test.elf $test
	riscv64-elf-objcopy -O binary $test.elf $test.bin
	./build/rv-test64 $test.bin > $test.out
done

