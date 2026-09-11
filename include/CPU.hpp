#pragma once

#include <iomanip>
#include <iostream>
#include <array>
#include <cstdint>
#include <sstream>
#include <vector>

#include "types.hpp"
#include "utils.hpp"
#include "instruction.hpp"

namespace riscv {

template<RegisterType T>
class CPU {
	inline void addi(DecodedInstruction<T> inst) {
		writeReg(inst.rd, readX(inst.rs1) + inst.imm);
	}

	inline void xori(DecodedInstruction<T> inst) {
		writeReg(inst.rd, readX(inst.rs1) ^ inst.imm);
	}

	inline void ori(DecodedInstruction<T> inst) {
		writeReg(inst.rd, readX(inst.rs1) | inst.imm);
	}

	inline void andi(DecodedInstruction<T> inst) {
		writeReg(inst.rd, readX(inst.rs1) & inst.imm);
	}

	inline void slli(DecodedInstruction<T> inst) {
		writeReg(inst.rd, readX(inst.rs1) << inst.shiftAmt);
	}

	inline void srli(DecodedInstruction<T> inst) {
		writeReg(inst.rd, readX(inst.rs1) >> inst.shiftAmt);
	}

	inline void srai(DecodedInstruction<T> inst) {
		writeReg(inst.rd, signExtend<T>(
			readX(inst.rs1) >> inst.shiftAmt, 
			xlen<T>() - inst.shiftAmt));
	}

	inline void sltiu(DecodedInstruction<T> inst) {
		writeReg(inst.rd, readX(inst.rs1) < inst.imm);
	}

public:
	T readPC() const {
		return pc;
	}

	T readX(u32 reg) const {
		return x[reg];
	}

	void writeReg(u32 reg, T value) {
		if (reg != 0) {
			x[reg] = value;
		}
	}

	char execute(DecodedInstruction<T> inst) {
		switch (inst.type) {
		case InstructionType::ADDI:
			addi(inst);
			break;
		case InstructionType::XORI:
			xori(inst);
			break;
		case InstructionType::ORI:
			ori(inst);
			break;
		case InstructionType::ANDI:
			andi(inst);
			break;
		case InstructionType::SLLI:
			slli(inst);
			break;
		case InstructionType::SRLI:
			srli(inst);
			break;
		case InstructionType::SRAI:
			srai(inst);
			break;
		case InstructionType::SLTIU:
			sltiu(inst);
			break;
		default:
			return 0;
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
		auto rawInst = fetch(dram);
		pc += 4;

		auto inst = decodeInstruction<T>(rawInst);
		auto successCode = execute(inst);

		return successCode;
	}

private:
	std::array<T, 32> x{};
	T pc{};
};

}

