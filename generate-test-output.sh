#!/bin/bash

for test in `find tests/rv32i -name '*.s' | sort`; do
	riscv64-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -Wl,-e,_start -Wl,-Ttext=0x0 -o $test.elf $test
	riscv64-elf-objcopy -O binary $test.elf $test.bin
	./build/test-rv32i $test.bin > $test.out
done

for test in `find tests/rv64i -name '*.s' | sort`; do
	riscv64-elf-gcc -march=rv64i -mabi=lp64 -nostdlib -Wl,-e,_start -Wl,-Ttext=0x0 -o $test.elf $test
	riscv64-elf-objcopy -O binary $test.elf $test.bin
	./build/test-rv64i $test.bin > $test.out
done

