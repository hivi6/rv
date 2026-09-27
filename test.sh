#/bin/bash

set -euo pipefail

for test_type in `ls tests | xargs`; do
	echo "Testing $test_type"
	all_passed=1
	for test in `find tests/$test_type -name '*.s' | sort`; do
		abi=ilp32
		if [ "$test_type" == "rv64i" ]; then
			abi=lp64
		fi

		riscv64-elf-gcc -march=$test_type -mabi=${abi} -nostdlib \
			-Wl,-e,_start -Wl,-Ttext=0x0 -o $test.elf $test
		riscv64-elf-objcopy -O binary $test.elf $test.bin
		./build/test-$test_type $test.bin 2>&1 \
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

