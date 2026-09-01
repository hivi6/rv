#include <fstream>

#include "CPU.hpp"

std::vector<riscv::u8> loadBin(std::string filepath) {
	std::ifstream file(filepath, std::ios::binary | std::ios::ate);
	if (!file.is_open()) {
		std::cerr << "Error opening binary file!" << std::endl;
		return {};
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<riscv::u8> buffer(size);
	if (!file.read((char*)buffer.data(), size)) {
		std::cerr << "Error reading file" << std::endl;
		return {};
	}

	return buffer;
}

int main(int argc, const char **argv) {
	if (argc <= 1) {
		std::cerr << "ERROR: Expected rv <filepath>" << std::endl;
		return 1;
	}

	std::string filepath(argv[1]);
	auto dram = loadBin(filepath);

	riscv::CPU<riscv::u32> cpu;

	for (int step=1; ; step++) {
		if (cpu.readPC() >= dram.size()) break;

		std::cout << "STEP: " << step << std::endl;
		auto success = cpu.step(dram);
		cpu.printRegisters();
		std::cout << std::endl;

		if (!success) break;
	}
	
	return 0;
}

