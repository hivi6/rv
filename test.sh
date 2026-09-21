#/bin/bash

set -euo pipefail

for xlen in `echo "32 64" | xargs`; do
	echo "Testing r${xlen}"
	all_passed=1
	for test in `find tests/r${xlen} -name '*.s' | sort`; do
		abi=ilp32
		if [ "$xlen" == "64" ]; then
			abi=lp64
		fi

		riscv64-elf-gcc -march=rv${xlen}i -mabi=${abi} -nostdlib \
			-Wl,-e,_start -Wl,-Ttext=0x0 -o $test.elf $test
		riscv64-elf-objcopy -O binary $test.elf $test.bin
		./build/rv-test${xlen} $test.bin 2>&1 \
			| diff - $test.out &> /dev/null
		passed=$?

		if [ $passed == 0 ]; then
			echo PASSED - $test
		else
			echo FAILED - $test
			all_passed=0
		fi
	done

	if [ $all_passed == 1 ]; then
		echo PASSED
	else
		echo FAILED
	fi
	echo
done

