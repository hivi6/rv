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
	UNSUPPORTED_INSTRUCTION,
	UNEXECUTED_INSTRUCTION,
	INSTRUCTION_ADDRESS_MISALIGNED,
	LOAD_ACCESS_FAULT,
	STORE_ACCESS_FAULT,
	ENVIRONMENT_CALL,
	ENVIRONMENT_BREAK,
};

struct CPUError {
	CPUErrorType type;
	std::string msg;

	CPUError(CPUErrorType t, std::string m) : type{t}, msg{m} {}
};

template<RegisterType T>
class CPU {
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

	inline std::expected<T, CPUError> add(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) + readX(inst.rs2));
		return pc + 4;
	}

	inline std::expected<T, CPUError> sub(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) - readX(inst.rs2));
		return pc + 4;
	}

	inline std::expected<T, CPUError> sll(DecodedInstruction<T> inst) {
		// for 32 bits, only last 5 bits are used for shift amount
		// for 64 bits, onlt last 6 bits are used for shift amount
		const auto validMask = xlen<T>() - 1;
		const auto shamt = readX(inst.rs2) & validMask;
		writeX(inst.rd, readX(inst.rs1) << shamt);
		return pc + 4;
	}

	inline std::expected<T, CPUError> slt(DecodedInstruction<T> inst) {
		using signT = std::make_signed_t<T>;
		const auto lhs = std::bit_cast<signT>(readX(inst.rs1));
		const auto rhs = std::bit_cast<signT>(readX(inst.rs2));
		writeX(inst.rd, lhs < rhs);
		return pc + 4;
	}

	inline std::expected<T, CPUError> sltu(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) < readX(inst.rs2));
		return pc + 4;
	}

	inline std::expected<T, CPUError> _xor(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) ^ readX(inst.rs2));
		return pc + 4;
	}

	inline std::expected<T, CPUError> srl(DecodedInstruction<T> inst) {
		// for 32 bits, only last 5 bits are used for shift amount
		// for 64 bits, onlt last 6 bits are used for shift amount
		const auto validMask = xlen<T>() - 1;
		const auto shamt = readX(inst.rs2) & validMask;
		writeX(inst.rd, readX(inst.rs1) >> shamt);
		return pc + 4;
	}

	inline std::expected<T, CPUError> sra(DecodedInstruction<T> inst) {
		// for 32 bits, only last 5 bits are used for shift amount
		// for 64 bits, onlt last 6 bits are used for shift amount
		const auto validMask = xlen<T>() - 1;
		const auto shamt = readX(inst.rs2) & validMask;
		const auto res = readX(inst.rs1) >> shamt;
		const auto signEx = signExtend<T>(res, xlen<T>() - shamt);
		writeX(inst.rd, signEx);
		return pc + 4;
	}

	inline std::expected<T, CPUError> _or(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) | readX(inst.rs2));
		return pc + 4;
	}

	inline std::expected<T, CPUError> _and(DecodedInstruction<T> inst) {
		writeX(inst.rd, readX(inst.rs1) & readX(inst.rs2));
		return pc + 4;
	}

	inline std::expected<T, CPUError> fence(DecodedInstruction<T> inst) {
		// WORKAROUND: as we are just implementing a single threaded
		// syncronise emulator so fence instructions are not required
		// as by default memory consistency is guaranteed. Therefore,
		// FENCE, FENCE.TSO, PAUSE are just increment operation
		//
		// Resources:
		// - "Chapter 2" https://docs.riscv.org/reference/isa/_attachments/riscv-unprivileged.pdf
		//   To quote: 'The ISA was also designed to reduce the hardware 
		//   required in a minimal implementation. RV32I contains 40 
		//   unique instructions, though a simple implementation might 
		//   cover the ECALL/EBREAK instructions with a single SYSTEM 
		//   hardware instruction that always traps and might be able 
		//   to implement the FENCE instruction as a NOP, reducing base 
		//   instruction count to 38 total.'
		return pc + 4;
	}

	inline std::expected<T, CPUError> fenceTso(DecodedInstruction<T> inst) {
		return pc + 4;
	}

	inline std::expected<T, CPUError> pause(DecodedInstruction<T> inst) {
		return pc + 4;
	}

	inline std::expected<T, CPUError> ecall(DecodedInstruction<T> inst) {
		// TODO: to implement ecall, we need to make sure to implement
		// the previledge mode instructions, for now just returning
		// an error
		return std::unexpected(CPUError(
			CPUErrorType::ENVIRONMENT_CALL,
			"Environment call"));
	}

	inline std::expected<T, CPUError> ebreak(DecodedInstruction<T> inst) {
		// TODO: to implement ebreak, we need to make sure to implement
		// the previledge mode instructions, for now just returning
		// an error
		return std::unexpected(CPUError(
			CPUErrorType::ENVIRONMENT_BREAK,
			"Environment break"));
	}

	inline std::expected<T, CPUError> lwu(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"LWU not supported for r32"));
		}

		const auto address = readX(inst.rs1) + inst.imm;
		auto busLoad = bus.load<u32>(address);
		if (!busLoad) {
			return std::unexpected(CPUError(
				CPUErrorType::LOAD_ACCESS_FAULT,
				"LWU target address couldn't be loaded"));
		}

		const auto res = *busLoad;
		writeX(inst.rd, res);
		return pc + 4;
	}

	inline std::expected<T, CPUError> ld(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"LD not supported for r32"));
		}

		const auto address = readX(inst.rs1) + inst.imm;
		auto busLoad = bus.load<u64>(address);
		if (!busLoad) {
			return std::unexpected(CPUError(
				CPUErrorType::LOAD_ACCESS_FAULT,
				"LD target address couldn't be loaded"));
		}

		const auto res = signExtend<T>(*busLoad, 64);
		writeX(inst.rd, res);
		return pc + 4;
	}

	inline std::expected<T, CPUError> sd(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"SD not supported for r32"));
		}

		const auto address = readX(inst.rs1) + inst.imm;
		auto busStore = bus.store<u64>(address, readX(inst.rs2));
		if (!busStore) {
			return std::unexpected(CPUError(
				CPUErrorType::STORE_ACCESS_FAULT,
				"SD target address couldn't be stored"));
		}
		return pc + 4;
	}

	inline std::expected<T, CPUError> addiw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"ADDIW not supported for r32"));
		}

		const auto res = static_cast<u32>(readX(inst.rs1) + inst.imm);
		writeX(inst.rd, signExtend<T>(res, 32));
		return pc + 4;
	}

	inline std::expected<T, CPUError> slliw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"SLLIW not supported for r32"));
		}

		const auto res = static_cast<u32>(readX(inst.rs1) << inst.imm);
		writeX(inst.rd, signExtend<T>(res, 32));
		return pc + 4;
	}

	inline std::expected<T, CPUError> srliw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"SRLIW not supported for r32"));
		}

		const u32 word = static_cast<u32>(readX(inst.rs1));
		const u32 res = word >> inst.shiftAmt;
		writeX(inst.rd, signExtend<T>(res, 32));
		return pc + 4;
	}

	inline std::expected<T, CPUError> sraiw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"SRAIW not supported for r32"));
		}

		const u32 word = static_cast<u32>(readX(inst.rs1));
		const u32 res = word >> inst.shiftAmt;
		writeX(inst.rd, signExtend<T>(res, 32 - inst.shiftAmt));
		return pc + 4;
	}

	inline std::expected<T, CPUError> addw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"ADDW not supported for r32"));
		}

		const auto res = static_cast<u32>(readX(inst.rs1) 
			+ readX(inst.rs2));
		writeX(inst.rd, signExtend<T>(res, 32));
		return pc + 4;
	}

	inline std::expected<T, CPUError> subw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"SUBW not supported for r32"));
		}

		const auto res = static_cast<u32>(readX(inst.rs1) 
			- readX(inst.rs2));
		writeX(inst.rd, signExtend<T>(res, 32));
		return pc + 4;
	}

	inline std::expected<T, CPUError> sllw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"SLLW not supported for r32"));
		}

		const auto validMask = xlen<u32>() - 1;
		const auto shamt = readX(inst.rs2) & validMask;
		const u32 res = static_cast<u32>(readX(inst.rs1)) << shamt;
		writeX(inst.rd, signExtend<T>(res, 32));
		return pc + 4;
	}

	inline std::expected<T, CPUError> srlw(DecodedInstruction<T> inst) {
		if (xlen<T>() == 32) {
			return std::unexpected(CPUError(
				CPUErrorType::UNSUPPORTED_INSTRUCTION,
				"SRLW not supported for r32"));
		}

		const auto validMask = xlen<u32>() - 1;
		const auto shamt = readX(inst.rs2) & validMask;
		const u32 res = static_cast<u32>(readX(inst.rs1)) >> shamt;
		writeX(inst.rd, signExtend<T>(res, 32));
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
		case InstructionType::LUI:       return lui(inst);
		case InstructionType::AUIPC:     return auipc(inst);
		case InstructionType::JAL:       return jal(inst);
		case InstructionType::JALR:      return jalr(inst);
		case InstructionType::BEQ:       return beq(inst);
		case InstructionType::BNE:       return bne(inst);
		case InstructionType::BLT:       return blt(inst);
		case InstructionType::BGE:       return bge(inst);
		case InstructionType::BLTU:      return bltu(inst);
		case InstructionType::BGEU:      return bgeu(inst);
		case InstructionType::LB:        return lb(inst);
		case InstructionType::LH:        return lh(inst);
		case InstructionType::LW:        return lw(inst);
		case InstructionType::LBU:       return lbu(inst);
		case InstructionType::LHU:       return lhu(inst);
		case InstructionType::SB:        return sb(inst);
		case InstructionType::SH:        return sh(inst);
		case InstructionType::SW:        return sw(inst);
		case InstructionType::ADDI:      return addi(inst);
		case InstructionType::XORI:      return xori(inst);
		case InstructionType::ORI:       return ori(inst);
		case InstructionType::ANDI:      return andi(inst);
		case InstructionType::SLLI:      return slli(inst);
		case InstructionType::SRLI:      return srli(inst);
		case InstructionType::SRAI:      return srai(inst);
		case InstructionType::SLTIU:     return sltiu(inst);
		case InstructionType::SLTI:      return slti(inst);
		case InstructionType::ADD:       return add(inst);
		case InstructionType::SUB:       return sub(inst);
		case InstructionType::SLL:       return sll(inst);
		case InstructionType::SLT:       return slt(inst);
		case InstructionType::SLTU:      return sltu(inst);
		case InstructionType::XOR:       return _xor(inst);
		case InstructionType::SRL:       return srl(inst);
		case InstructionType::SRA:       return sra(inst);
		case InstructionType::OR:        return _or(inst);
		case InstructionType::AND:       return _and(inst);
		case InstructionType::FENCE:     return fence(inst);
		case InstructionType::FENCE_TSO: return fenceTso(inst);
		case InstructionType::PAUSE:     return pause(inst);
		case InstructionType::ECALL:     return ecall(inst);
		case InstructionType::EBREAK:    return ebreak(inst);
		case InstructionType::LWU:       return lwu(inst);
		case InstructionType::LD:        return ld(inst);
		case InstructionType::SD:        return sd(inst);
		case InstructionType::ADDIW:     return addiw(inst);
		case InstructionType::SLLIW:     return slliw(inst);
		case InstructionType::SRLIW:     return srliw(inst);
		case InstructionType::SRAIW:     return sraiw(inst);
		case InstructionType::ADDW:      return addw(inst);
		case InstructionType::SUBW:      return subw(inst);
		case InstructionType::SLLW:      return sllw(inst);
		case InstructionType::SRLW:      return srlw(inst);
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
		u32 funct7 = 0;

		if (opcode == 0b0110111) {
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
			else if (funct3 == 0b011)
				type = InstructionType::LD;
			else if (funct3 == 0b100)
				type = InstructionType::LBU;
			else if (funct3 == 0b101)
				type = InstructionType::LHU;
			else if (funct3 == 0b110)
				type = InstructionType::LWU;
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
			else if (funct3 == 0b011)
				type = InstructionType::SD;
		}
		else if (opcode == 0b0010011) {
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
		else if (opcode == 0b0110011) {
			funct3 = RType<T>::funct3(raw);
			funct7 = RType<T>::funct7(raw);

			if (funct3 == 0b000 && funct7 == 0b0000000)
				type = InstructionType::ADD;
			else if (funct3 == 0b000 && funct7 == 0b0100000)
				type = InstructionType::SUB;
			else if (funct3 == 0b001 && funct7 == 0b0000000)
				type = InstructionType::SLL;
			else if (funct3 == 0b010 && funct7 == 0b0000000)
				type = InstructionType::SLT;
			else if (funct3 == 0b011 && funct7 == 0b0000000)
				type = InstructionType::SLTU;
			else if (funct3 == 0b100 && funct7 == 0b0000000)
				type = InstructionType::XOR;
			else if (funct3 == 0b101 && funct7 == 0b0000000)
				type = InstructionType::SRL;
			else if (funct3 == 0b101 && funct7 == 0b0100000)
				type = InstructionType::SRA;
			else if (funct3 == 0b110 && funct7 == 0b0000000)
				type = InstructionType::OR;
			else if (funct3 == 0b111 && funct7 == 0b0000000)
				type = InstructionType::AND;
		}
		else if (opcode == 0b0001111) {
			funct3 = IType<T>::funct3(raw);

			if (funct3 == 0b000) {
				const auto fm = (raw >> 28);
				const auto pred = (raw >> 24) & 0b1111;
				const auto succ = (raw >> 20) & 0b1111;

				type = InstructionType::FENCE;

				if (fm == 0b1000 && pred == 0b0011 
					&& succ == 0b0011 && rs1 == 0b00000)
					type = InstructionType::FENCE_TSO;
				else if (fm == 0b0000 && pred == 0b0001
					&& succ == 0b0000 && rs1 == 0b00000)
					type = InstructionType::PAUSE;
			}
		}
		else if (opcode == 0b1110011) {
			imm = IType<T>::imm(raw);

			funct3 = IType<T>::funct3(raw);

			if (imm == 0b000000000000 && rs1 == 0b00000 &&
				funct3 == 0b000 && rd == 0b00000)
				type = InstructionType::ECALL;
			if (imm == 0b000000000001 && rs1 == 0b00000 &&
				funct3 == 0b000 && rd == 0b00000)
				type = InstructionType::EBREAK;
		}
		else if (opcode == 0b0011011) {
			imm = IType<T>::imm(raw);

			funct3 = IType<T>::funct3(raw);

			// make sure the shiftType is based on u32
			shiftType = IType<u32>::shiftType(raw);
			shiftAmt = IType<u32>::shiftAmt(raw);

			if (funct3 == 0b000)
				type = InstructionType::ADDIW;
			else if (funct3 == 0b001 && shiftType == 0b0000000)
				type = InstructionType::SLLIW;
			else if (funct3 == 0b101 && shiftType == 0b0000000)
				type = InstructionType::SRLIW;
			else if (funct3 == 0b101 && shiftType == 0b0100000)
				type = InstructionType::SRAIW;
		}
		else if (opcode == 0b0111011) {
			funct3 = RType<T>::funct3(raw);
			funct7 = RType<T>::funct7(raw);

			if (funct3 == 0b000 && funct7 == 0b0000000)
				type = InstructionType::ADDW;
			else if (funct3 == 0b000 && funct7 == 0b0100000)
				type = InstructionType::SUBW;
			else if (funct3 == 0b001 && funct7 == 0b0000000)
				type = InstructionType::SLLW;
			else if (funct3 == 0b101 && funct7 == 0b0000000)
				type = InstructionType::SRLW;
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
			.funct7 = funct7,
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
