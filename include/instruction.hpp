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
struct UType {
	static constexpr u32 opcode(u32 i)  { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)      { return (i >> 7) & 0x1f;  }
	static constexpr T imm(u32 i) { 
		return signExtend<T>(i & 0xfffff000u, 32);
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

template<RegisterType T>
DecodedInstruction<T> decodeInstruction(u32 raw) {
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
			if (shiftType == 0b0100000 && xlen<T>() == 32 || 
				shiftType == 0b010000 && xlen<T>() == 64)
				type = InstructionType::SRAI;
		}
		else if (funct3 == 0b011)
			type = InstructionType::SLTIU;
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

}

