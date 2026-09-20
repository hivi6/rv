#pragma once

#include "types.hpp"
#include "utils.hpp"

namespace riscv {

template<RegisterType T>
struct RType {
	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)     { return (i >> 7) & 0x1f;  }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr u32 rs2(u32 i)    { return (i >> 20) & 0x1f; }
	static constexpr u32 funct7(u32 i) { return (i >> 25);        }
};

template<RegisterType T>
struct IType {
	static constexpr u32 opcode(u32 i)   { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)       { return (i >> 7) & 0x1f;  }
	static constexpr u32 funct3(u32 i)   { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)      { return (i >> 15) & 0x1f; }
	static constexpr u32 shiftAmtLen() {
		return sizeof(T) == 4 ? 5 : 6;
	}
	static constexpr u32 shiftAmt(u32 i) {
		return (i >> 20) & ((u32{1} << shiftAmtLen()) - 1);
	}
	static constexpr u32 shiftType(u32 i) {
		return i >> (20 + shiftAmtLen());
	}
	static constexpr T imm(u32 i)    {
		return signExtend<T>(i >> 20, 12);
	}
};

template<RegisterType T>
struct SType {
	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr u32 rs2(u32 i)    { return (i >> 20) & 0x1f; }
	static constexpr T imm(u32 i)    {
		const T raw = ((i >> 25) << 5) | ((i >> 7) & 0x1f);
		return signExtend<T>(raw, 12);
	}
};

template<RegisterType T>
struct BType {
	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr u32 rs2(u32 i)    { return (i >> 20) & 0x1f; }
	static constexpr T imm(u32 i) {
		const auto imm_11   = (i >> 7) & 0b1;
		const auto imm_4_1  = (i >> 8) & 0b1111;
		const auto imm_10_5 = (i >> 25) & 0b111111;
		const auto imm_12   = (i >> 31) & 0b1;
		const auto res = (imm_12 << 12) | (imm_11 << 11) 
			| (imm_10_5 << 5) | (imm_4_1 << 1);
		return signExtend<T>(res, 13);
	}
};

template<RegisterType T>
struct UType {
	static constexpr u32 opcode(u32 i)  { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)      { return (i >> 7) & 0x1f;  }
	static constexpr T imm(u32 i) { 
		return signExtend<T>(i & 0xfffff000u, 32);
	}
};

template<RegisterType T>
struct JType {
	static constexpr u32 opcode(u32 i) { return i & 0x7f; }
	static constexpr u32 rd(u32 i)     { return (i >> 7) & 0x1f;  }
	static constexpr T imm(u32 i) {
		const auto imm_10_1  = (i >> 21) & 0b1111111111;
		const auto imm_20    = (i >> 31) & 0b1;
		const auto imm_11    = (i >> 20) & 0b1;
		const auto imm_19_12 = (i >> 12) & 0b11111111;
		const auto res = (imm_20 << 20) | (imm_19_12 << 12)
			| (imm_11 << 11) | (imm_10_1 << 1);
		return signExtend<T>(res, 21);
	}
};

enum class InstructionType {
	INVALID = 0, // custom instruction if there is something wrong
	ADDI, 
	XORI, 
	ORI, 
	ANDI, 
	SLLI, 
	SRLI, 
	SRAI,
	SLTIU,
	SLTI,
	LUI,
	AUIPC,
	JAL,
	JALR,
	BEQ,
	BNE,
	BLT,
	BGE,
	BLTU,
	BGEU,
	LB,
	LH,
	LW,
	LBU,
	LHU,
	SB,
	SH,
	SW,
};

template<RegisterType T>
struct DecodedInstruction {
	InstructionType type;

	u32 raw;
	u32 opcode;
	u32 rd;
	u32 rs1;
	u32 rs2;
	T imm;

	u32 funct3;
	u32 shiftType;
	u32 shiftAmt;
};

}

