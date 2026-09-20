#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <vector>

#include "types.hpp"
#include "utils.hpp"
#include "instruction.hpp"

namespace riscv {

enum class CPUErrorType {
	INVALID_INSTRUCTION,
	UNEXECUTED_INSTRUCTION,
	INSTRUCTION_ADDRESS_MISALIGNED,
};

struct CPUError {
	CPUErrorType type;
	std::string msg;

	CPUError(CPUErrorType t, std::string m) : type{t}, msg{m} {}
};

template<RegisterType T>
class CPU {
	inline std::expected<T, CPUError> addi(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) + inst.imm);
		return pc + 4;
	}

	inline std::expected<T, CPUError> xori(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) ^ inst.imm);
		return pc + 4;
	}

	inline std::expected<T, CPUError> ori(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) | inst.imm);
		return pc + 4;
	}

	inline std::expected<T, CPUError> andi(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) & inst.imm);
		return pc + 4;
	}

	inline std::expected<T, CPUError> slli(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) << inst.shiftAmt);
		return pc + 4;
	}

	inline std::expected<T, CPUError> srli(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) >> inst.shiftAmt);
		return pc + 4;
	}

	inline std::expected<T, CPUError> srai(DecodedInstruction<T> inst) {
		writeX(inst.rd, signExtend<T>(
			readX(inst.rs1) >> inst.shiftAmt, 
			xlen<T>() - inst.shiftAmt));
		return pc + 4;
	}

	inline std::expected<T, CPUError> sltiu(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) < inst.imm);
		return pc + 4;
	}

	inline std::expected<T, CPUError> slti(DecodedInstruction<T> inst) {
		using signT = std::make_signed_t<T>;
		const auto lhs = std::bit_cast<signT>(readX(inst.rs1));
		const auto rhs = std::bit_cast<signT>(inst.imm);
		writeX(inst.rd, lhs < rhs);
		return pc + 4;
	}

	inline std::expected<T, CPUError> lui(DecodedInstruction<T> inst) {
		writeX(inst.rd, inst.imm);
		return pc + 4;
	}

	inline std::expected<T, CPUError> auipc(DecodedInstruction<T> inst) {
		writeX(inst.rd, inst.imm + pc);
		return pc + 4;
	}

	inline std::expected<T, CPUError> jal(DecodedInstruction<T> inst) {
		const auto nextPC = pc + inst.imm;
		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"JAL target address is not 4-byte aligned"));
		}

		writeX(inst.rd, pc + 4);
		return nextPC;
	}

public:
	T readPC() const {
		return pc;
	}

	T readX(u32 reg) const {
		return x[reg];
	}

	void writeX(u32 reg, T value) {
		if (reg != 0) {
			x[reg] = value;
		}
	}

	std::expected<T, CPUError> execute(DecodedInstruction<T> inst) {
		switch (inst.type) {
		case InstructionType::ADDI:  return addi(inst);
		case InstructionType::XORI:  return xori(inst);
		case InstructionType::ORI:   return ori(inst);
		case InstructionType::ANDI:  return andi(inst);
		case InstructionType::SLLI:  return slli(inst);
		case InstructionType::SRLI:  return srli(inst);
		case InstructionType::SRAI:  return srai(inst);
		case InstructionType::SLTIU: return sltiu(inst);
		case InstructionType::SLTI:  return slti(inst);
		case InstructionType::LUI:   return lui(inst);
		case InstructionType::AUIPC: return auipc(inst);
		case InstructionType::JAL:   return jal(inst);
		default: {
			std::string errorMsg = 
				"instruction couldn't be executed";
			return std::unexpected(CPUError(
				CPUErrorType::UNEXECUTED_INSTRUCTION, 
				errorMsg));
		}
		}
		return {};
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
		else if (opcode == 0b0110111) {
			imm = UType<T>::imm(raw);
			type = InstructionType::LUI;
		}
		else if (opcode == 0b0010111) {
			imm = UType<T>::imm(raw);
			type = InstructionType::AUIPC;
		}
		else if (opcode == 0b1101111) {
			imm = JType<T>::imm(raw);
			type = InstructionType::JAL;
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

	std::expected<void, CPUError> step(const std::vector<u8> &dram) {
		auto rawInst = fetch(dram);

		auto inst = decode(rawInst);
		if (inst.type == InstructionType::INVALID) {
			std::string errorMsg = "decoding failed: " 
				+ toHex(rawInst);
			return std::unexpected(CPUError(
				CPUErrorType::INVALID_INSTRUCTION, errorMsg));
		}

		auto res = execute(inst);
		if (!res) {
			return std::unexpected(res.error());
		}

		pc = *res;

		return {};
	}

private:
	std::array<T, 32> x{};
	T pc{};
};

}

