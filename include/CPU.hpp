#pragma once

#include <iomanip>
#include <iostream>
#include <array>
#include <cstdint>
#include <sstream>
#include <type_traits>
#include <vector>

using u8  = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

#define CHECK_XLEN_TYPE(type) \
	static_assert(std::is_same<type, u32>::value || \
	std::is_same<type, u64>::value, \
	"CPU only supports u32 or u64");

enum class Opcode : u32 {
	OpImm = 0x13, // ADDI, SLTI, SLTIU, ANDI, ORI, XORI, SLLI, SRLI, SRAI
};

template<typename RegType>
constexpr RegType signExtend(RegType value, u32 bits) {
	CHECK_XLEN_TYPE(RegType);

	const RegType signBit = RegType{1} << (bits - 1);
	return (value ^ signBit) - signBit;
}

template<typename RegType>
struct RType {
	CHECK_XLEN_TYPE(RegType);

	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)     { return (i >> 7) & 0x1f;  }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr u32 rs2(u32 i)    { return (i >> 20) & 0x1f; }
	static constexpr u32 funct7(u32 i) { return (i >> 25);        }
};

template<typename RegType>
struct IType {
	CHECK_XLEN_TYPE(RegType);

	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)     { return (i >> 7) & 0x1f;  }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr RegType imm(u32 i)    {
		return signExtend<RegType>(i >> 20, 12);
	}
};

template<typename RegType>
struct SType {
	CHECK_XLEN_TYPE(RegType);

	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr u32 rs2(u32 i)    { return (i >> 20) & 0x1f; }
	static constexpr RegType imm(u32 i)    {
		const RegType raw = ((i >> 25) << 5) | ((i >> 7) & 0x1f);
		return signExtend<RegType>(raw, 12);
	}
};

template<typename RegType>
struct UType {
	CHECK_XLEN_TYPE(RegType);

	static constexpr u32 opcode(u32 i)  { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)      { return (i >> 7) & 0x1f;  }
	static constexpr RegType imm(u32 i) { 
		return signExtend<RegType>(i & 0xfffff000u, 32);
	}
};

template <typename RegType>
class CPU {
	CHECK_XLEN_TYPE(RegType);
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

public:
	static constexpr u32 xlen() {
		return sizeof(RegType) * 8;
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
			case 0b000: addi(inst); break;
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
		auto inst = fetch(dram + pc);
		pc += 4;
		return execute(inst);
	}

	void printRegisters() {
		cout << "pc : " << toHex(pc) << endl << endl;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 4; j++) {
				int reg = i * 4 + j;
				auto regStr = to_string(reg);
				if (regStr.size() <= 1) regStr.push_back(' ');
				cout << "x" << regStr << " : " 
					<< toHex(x[reg]) << " ";
			}
			cout << endl;
		}
	}

private:
	std::array<RegType, 32> x{};
	RegType pc{};
};

