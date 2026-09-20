#include <iostream>
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

template<riscv::RegisterType T>
void printRegisters(const riscv::CPU<T> &cpu) {
	std::cout << "pc : " << riscv::toHex<T>(cpu.readPC()) << std::endl;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			int reg = i * 4 + j;
			auto regStr = std::to_string(reg);
			if (regStr.size() <= 1) regStr.push_back(' ');
			std::cout << "x" << regStr << " : " 
				<< riscv::toHex<T>(cpu.readX(reg)) << " ";
		}
		std::cout << std::endl;
	}
}


int main(int argc, const char **argv) {
	if (argc <= 1) {
		std::cerr << "ERROR: Expected rv <filepath>" << std::endl;
		return 1;
	}

	std::string filepath(argv[1]);

	riscv::Bus bus(loadBin(filepath));
	riscv::CPU<riscv::u32> cpu(bus);

	for (int step=1; ; step++) {
		if (cpu.readPC() >= bus.binSize()) break;

		std::cout << "STEP: " << step << std::endl;
		auto success = cpu.step();
		if (!success) {
			auto err = success.error();
			std::cerr << "ERROR: " << err.msg << std::endl;
		}

		printRegisters(cpu);
		std::cout << std::endl;

		if (!success) {
			return 1;
		}
	}
	
	return 0;
}

