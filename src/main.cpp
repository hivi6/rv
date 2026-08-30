#include <iostream>
#include <cstdint>
using namespace std;

#include "CPU.hpp"

int main() {
	CPU<uint32_t> cpu;

	cpu.printRegisters();

	return 0;
}

