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

	inline void slti(DecodedInstruction<T> inst) {
		using signT = std::make_signed_t<T>;
		const auto lhs = std::bit_cast<signT>(readX(inst.rs1));
		const auto rhs = std::bit_cast<signT>(inst.imm);
		writeReg(inst.rd, lhs < rhs);
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
		case InstructionType::SLTI:
			slti(inst);
			break;
		default:
			return 0;
		}
		return 1;
	}

	static DecodedInstruction<T> decode(u32 raw) {
		InstructionType type = InstructionType::INVALID;

		auto opcode = IType<T>::opcode(raw);
		auto rd = IType<T>::rd(raw);
		auto rs1 = IType<T>::rs1(raw);
		auto rs2 = SType<T>::rs2(raw);
		T imm = 0;

		u32 funct3 = 0;
		u32 shiftType = 0;
		u32 shiftAmt = 0;

		// OP-IMM
		if (opcode == 0b0010011) {
			imm = IType<T>::imm(raw);
		
			funct3 = IType<T>::funct3(raw);
			shiftType = IType<T>::shiftType(raw);
			shiftAmt = IType<T>::shiftAmt(raw);

			if (funct3 == 0b000) 
				type = InstructionType::ADDI;
			else if (funct3 == 0b100) 
				type = InstructionType::XORI;
			else if (funct3 == 0b110) 
				type = InstructionType::ORI;
			else if (funct3 == 0b111) 
				type = InstructionType::ANDI;
			else if (funct3 == 0b001 && shiftType == 0)
				type = InstructionType::SLLI;
			else if (funct3 == 0b101 && shiftType == 0)
				type = InstructionType::SRLI;
			else if (funct3 == 0b101) {
				if (shiftType == 0b0100000 
					&& xlen<T>() == 32 
					|| shiftType == 0b010000 
					&& xlen<T>() == 64)
					type = InstructionType::SRAI;
			}
			else if (funct3 == 0b011)
				type = InstructionType::SLTIU;
			else if (funct3 == 0b010)
				type = InstructionType::SLTI;
		}

		return DecodedInstruction<T> {
			.type = type,

			.raw = raw,
			.opcode = opcode,
			.rd = rd,
			.rs1 = rs1,
			.rs2 = rs2,
			.imm = imm,

			.funct3 = funct3,
			.shiftType = shiftType,
			.shiftAmt = shiftAmt,
		};
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

		auto inst = decode(rawInst);
		auto successCode = execute(inst);

		return successCode;
	}

private:
	std::array<T, 32> x{};
	T pc{};
};

}

