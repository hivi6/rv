#pragma once

#include <array>
#include <cstdint>
#include <expected>
#include <vector>

#include "types.hpp"
#include "utils.hpp"
#include "instruction.hpp"
#include "bus.hpp"

namespace riscv {

enum class CPUStepState {
	RUNNING,
	HALTED,
};

enum class CPUErrorType {
	INSTRUCTION_ACCESS_FAULT,
	INVALID_INSTRUCTION,
	UNEXECUTED_INSTRUCTION,
	INSTRUCTION_ADDRESS_MISALIGNED,
	LOAD_ACCESS_FAULT,
	STORE_ACCESS_FAULT,
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

	inline std::expected<T, CPUError> jalr(DecodedInstruction<T> inst) {
		const auto nextPC = (readX(inst.rs1) + inst.imm) & ~T{1};
		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"JALR target address is not 4-byte aligned"));
		}

		writeX(inst.rd, pc + 4);
		return nextPC;
	}

	inline std::expected<T, CPUError> beq(DecodedInstruction<T> inst) {
		const auto nextPC = (readX(inst.rs1) == readX(inst.rs2) 
			? pc + inst.imm : pc + 4);

		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"BEQ target address is not 4-byte aligned"));
		}

		return nextPC;
	}

	inline std::expected<T, CPUError> bne(DecodedInstruction<T> inst) {
		const auto nextPC = (readX(inst.rs1) != readX(inst.rs2) 
			? pc + inst.imm : pc + 4);

		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"BNE target address is not 4-byte aligned"));
		}

		return nextPC;
	}

	inline std::expected<T, CPUError> blt(DecodedInstruction<T> inst) {
		using signT = std::make_signed_t<T>;
		const auto lhs = std::bit_cast<signT>(readX(inst.rs1));
		const auto rhs = std::bit_cast<signT>(readX(inst.rs2));
		const auto nextPC = (lhs < rhs ? pc + inst.imm : pc + 4);

		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"BLT target address is not 4-byte aligned"));
		}

		return nextPC;
	}

	inline std::expected<T, CPUError> bge(DecodedInstruction<T> inst) {
		using signT = std::make_signed_t<T>;
		const auto lhs = std::bit_cast<signT>(readX(inst.rs1));
		const auto rhs = std::bit_cast<signT>(readX(inst.rs2));
		const auto nextPC = (lhs >= rhs ? pc + inst.imm : pc + 4);

		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"BGE target address is not 4-byte aligned"));
		}

		return nextPC;
	}

	inline std::expected<T, CPUError> bltu(DecodedInstruction<T> inst) {
		const auto lhs = readX(inst.rs1);
		const auto rhs = readX(inst.rs2);
		const auto nextPC = (lhs < rhs ? pc + inst.imm : pc + 4);

		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"BLTU target address is not 4-byte aligned"));
		}

		return nextPC;
	}

	inline std::expected<T, CPUError> bgeu(DecodedInstruction<T> inst) {
		const auto lhs = readX(inst.rs1);
		const auto rhs = readX(inst.rs2);
		const auto nextPC = (lhs >= rhs ? pc + inst.imm : pc + 4);

		if (nextPC % T{4} != 0) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ADDRESS_MISALIGNED,
				"BGEU target address is not 4-byte aligned"));
		}

		return nextPC;
	}

	inline std::expected<T, CPUError> lb(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busLoad = bus.load<u8>(address);
		if (!busLoad) {
			return std::unexpected(CPUError(
				CPUErrorType::LOAD_ACCESS_FAULT,
				"LB target address couldn't be loaded"));
		}

		const auto res = signExtend<T>(T{*busLoad}, 8);
		writeX(inst.rd, res);
		return pc + 4;
	}

	inline std::expected<T, CPUError> lh(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busLoad = bus.load<u16>(address);
		if (!busLoad) {
			return std::unexpected(CPUError(
				CPUErrorType::LOAD_ACCESS_FAULT,
				"LH target address couldn't be loaded"));
		}

		const auto res = signExtend<T>(T{*busLoad}, 16);
		writeX(inst.rd, res);
		return pc + 4;
	}

	inline std::expected<T, CPUError> lw(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busLoad = bus.load<u32>(address);
		if (!busLoad) {
			return std::unexpected(CPUError(
				CPUErrorType::LOAD_ACCESS_FAULT,
				"LW target address couldn't be loaded"));
		}

		const auto res = signExtend<T>(T{*busLoad}, 32);
		writeX(inst.rd, res);
		return pc + 4;
	}

	inline std::expected<T, CPUError> lbu(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busLoad = bus.load<u8>(address);
		if (!busLoad) {
			return std::unexpected(CPUError(
				CPUErrorType::LOAD_ACCESS_FAULT,
				"LBU target address couldn't be loaded"));
		}

		const auto res = *busLoad;
		writeX(inst.rd, res);
		return pc + 4;
	}

	inline std::expected<T, CPUError> lhu(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busLoad = bus.load<u16>(address);
		if (!busLoad) {
			return std::unexpected(CPUError(
				CPUErrorType::LOAD_ACCESS_FAULT,
				"LHU target address couldn't be loaded"));
		}

		const auto res = *busLoad;
		writeX(inst.rd, res);
		return pc + 4;
	}

	inline std::expected<T, CPUError> sb(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busStore = bus.store<u8>(address, readX(inst.rs2));
		if (!busStore) {
			return std::unexpected(CPUError(
				CPUErrorType::STORE_ACCESS_FAULT,
				"SB target address couldn't be stored"));
		}
		return pc + 4;
	}

	inline std::expected<T, CPUError> sh(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busStore = bus.store<u16>(address, readX(inst.rs2));
		if (!busStore) {
			return std::unexpected(CPUError(
				CPUErrorType::STORE_ACCESS_FAULT,
				"SH target address couldn't be stored"));
		}
		return pc + 4;
	}

	inline std::expected<T, CPUError> sw(DecodedInstruction<T> inst) {
		const auto address = readX(inst.rs1) + inst.imm;
		auto busStore = bus.store<u32>(address, readX(inst.rs2));
		if (!busStore) {
			return std::unexpected(CPUError(
				CPUErrorType::STORE_ACCESS_FAULT,
				"SW target address couldn't be stored"));
		}
		return pc + 4;
	}

public:
	CPU(Bus& b): bus{b} {}

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
		case InstructionType::JALR:  return jalr(inst);
		case InstructionType::BEQ:   return beq(inst);
		case InstructionType::BNE:   return bne(inst);
		case InstructionType::BLT:   return blt(inst);
		case InstructionType::BGE:   return bge(inst);
		case InstructionType::BLTU:  return bltu(inst);
		case InstructionType::BGEU:  return bgeu(inst);
		case InstructionType::LB:    return lb(inst);
		case InstructionType::LH:    return lh(inst);
		case InstructionType::LW:    return lw(inst);
		case InstructionType::LBU:   return lbu(inst);
		case InstructionType::LHU:   return lhu(inst);
		case InstructionType::SB:    return sb(inst);
		case InstructionType::SH:    return sh(inst);
		case InstructionType::SW:    return sw(inst);
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
		else if (opcode == 0b1100111) {
			imm = IType<T>::imm(raw);
		
			funct3 = IType<T>::funct3(raw);

			if (funct3 == 0b000)
				type = InstructionType::JALR;
		}
		else if (opcode == 0b1100011) {
			imm = BType<T>::imm(raw);

			funct3 = BType<T>::funct3(raw);

			if (funct3 == 0b000)
				type = InstructionType::BEQ;
			else if (funct3 == 0b001)
				type = InstructionType::BNE;
			else if (funct3 == 0b100)
				type = InstructionType::BLT;
			else if (funct3 == 0b101)
				type = InstructionType::BGE;
			else if (funct3 == 0b110)
				type = InstructionType::BLTU;
			else if (funct3 == 0b111)
				type = InstructionType::BGEU;
		}
		else if (opcode == 0b0000011) {
			imm = IType<T>::imm(raw);
			
			funct3 = IType<T>::funct3(raw);

			if (funct3 == 0b000)
				type = InstructionType::LB;
			else if (funct3 == 0b001)
				type = InstructionType::LH;
			else if (funct3 == 0b010)
				type = InstructionType::LW;
			else if (funct3 == 0b100)
				type = InstructionType::LBU;
			else if (funct3 == 0b101)
				type = InstructionType::LHU;
		}
		else if (opcode == 0b0100011) {
			imm = SType<T>::imm(raw);

			funct3 = SType<T>::funct3(raw);

			if (funct3 == 0b000)
				type = InstructionType::SB;
			else if (funct3 == 0b001)
				type = InstructionType::SH;
			else if (funct3 == 0b010)
				type = InstructionType::SW;
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


	std::expected<u32, CPUError> fetch() {
		auto res = bus.load<u32>(pc);
		if (!res) {
			return std::unexpected(CPUError(
				CPUErrorType::INSTRUCTION_ACCESS_FAULT,
				"load address is outside mapped memory"));
		}
		return *res;
	}

	std::expected<void, CPUError> step() {
		auto rawInst = fetch();
		if (!rawInst) {
			return std::unexpected(rawInst.error());
		}

		auto inst = decode(*rawInst);
		if (inst.type == InstructionType::INVALID) {
			std::string errorMsg = "decoding failed: " 
				+ toHex(*rawInst);
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
	Bus& bus;
};

}
