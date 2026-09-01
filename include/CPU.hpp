#pragma once

#include <iomanip>
#include <iostream>
#include <array>
#include <cstdint>
#include <sstream>
#include <type_traits>
#include <vector>

#include "types.hpp"
#include "utils.hpp"
#include "instruction_formats.hpp"

namespace riscv {

enum class Opcode : u32 {
	// ADDI, XORI, ORI, ANDI, SLLI, SRLI, SRAI
	// SLTIU
	OpImm = 0b0010011,
};

template<typename RegType, typename = std::enable_if_t<IsRegType<RegType>>>
class CPU {
	using R = RType<RegType>;
	using I = IType<RegType>;
	using S = SType<RegType>;
	using U = UType<RegType>;

	template <typename T>
	std::string toHex(T value) {
		std::ostringstream out;

		out << "0x"
		    << std::hex
		    << std::setw(sizeof(T) * 2)
		    << std::setfill('0')
		    << value;

		return out.str();
	}

	inline void addi(u32 inst) {
		writeReg(I::rd(inst), readReg(I::rs1(inst)) + I::imm(inst));
	}

	inline void xori(u32 inst) {
		writeReg(I::rd(inst), readReg(I::rs1(inst)) ^ I::imm(inst));
	}

	inline void ori(u32 inst) {
		writeReg(I::rd(inst), readReg(I::rs1(inst)) | I::imm(inst));
	}

	inline void andi(u32 inst) {
		writeReg(I::rd(inst), readReg(I::rs1(inst)) & I::imm(inst));
	}

	inline void slli(u32 inst) {
		writeReg(I::rd(inst), readReg(I::rs1(inst)) << I::shamt(inst));
	}

	inline void srli(u32 inst) {
		writeReg(I::rd(inst), readReg(I::rs1(inst)) >> I::shamt(inst));
	}

	inline void srai(u32 inst) {
		auto rs1Val = readReg(I::rs1(inst));
		auto shamt = I::shamt(inst);

		writeReg(I::rd(inst), 
			signExtend<RegType>(rs1Val >> shamt, xlen() - shamt));
	}

	inline void sltiu(u32 inst) {
		writeReg(I::rd(inst), readReg(I::rs1(inst)) < I::imm(inst));
	}

public:
	static constexpr u32 xlen() {
		return sizeof(RegType) * 8;
	}

	RegType readPC() const {
		return pc;
	}

	RegType readReg(u32 reg) const {
		return x[reg];
	}

	void writeReg(u32 reg, RegType value) {
		if (reg != 0) {
			x[reg] = value;
		}
	}

	char execute(u32 inst) {
		switch (static_cast<Opcode>(R::opcode(inst))) {
		case Opcode::OpImm: {
			switch (I::funct3(inst)) {
			case 0b000: {
				addi(inst); 
				break;
			}
			case 0b100: {
				xori(inst); 
				break;
			}
			case 0b110: {
				ori(inst); 
				break;
			}
			case 0b111: {
				andi(inst); 
				break;
			}
			case 0b001: {
				if (I::shiftType(inst) == 0)
					slli(inst);
				else
					return 0; // invalid instruction
				break;
			}
			case 0b101: {
				if (I::shiftType(inst) == 0)
					srli(inst);
				else if ((I::shiftType(inst) == 0b0100000 
					&& xlen() == 32)
					|| (I::shiftType(inst) == 0b010000 
					&& xlen() == 64))
					srai(inst);
				else
					return 0; // invalid instruction
				break;
			}
			case 0b011: {
				sltiu(inst); 
				break;
			}
			default: return 0;
			}
			break;
		}
		default: return 0;
		}

		return 1;
	}

	u32 fetch(const std::vector<u8> &dram) {
		// TODO: Support for both little endian and big endian
		// Current supporting only little endian
		return ((u32) dram[pc])
			| (((u32) dram[pc + 1]) << 8)
			| (((u32) dram[pc + 2]) << 16)
			| (((u32) dram[pc + 3]) << 24);
	}

	char step(const std::vector<u8> &dram) {
		auto inst = fetch(dram);
		pc += 4;
		
		auto successCode = execute(inst);
		if (!successCode) {
			std::cout << "Invalid instruction: " 
				<< toHex(inst) << std::endl;
		}
		return successCode;
	}

	void printRegisters() {
		std::cout << "pc : " << toHex(pc) << std::endl;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 4; j++) {
				int reg = i * 4 + j;
				auto regStr = std::to_string(reg);
				if (regStr.size() <= 1) regStr.push_back(' ');
				std::cout << "x" << regStr << " : " 
					<< toHex(x[reg]) << " ";
			}
			std::cout << std::endl;
		}
	}

private:
	std::array<RegType, 32> x{};
	RegType pc{};
};

}

