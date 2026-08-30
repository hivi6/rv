#include <iostream>
#include <cstdint>
using namespace std;

#include "CPU.hpp"

int main() {
	CPU<uint32_t> cpu;
	cout << "xlen: " << cpu.xlen() << endl;

	return 0;
}

