#pragma once

#include "types.hpp"
#include "utils.hpp"

namespace riscv {

template<typename RegType, typename = std::enable_if_t<IsRegType<RegType>>>
struct RType {
	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)     { return (i >> 7) & 0x1f;  }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr u32 rs2(u32 i)    { return (i >> 20) & 0x1f; }
	static constexpr u32 funct7(u32 i) { return (i >> 25);        }
};

template<typename RegType, typename = std::enable_if_t<IsRegType<RegType>>>
struct IType {
	static constexpr u32 opcode(u32 i)   { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)       { return (i >> 7) & 0x1f;  }
	static constexpr u32 funct3(u32 i)   { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)      { return (i >> 15) & 0x1f; }
	static constexpr u32 shamtLen() { 
		return sizeof(RegType) == 4 ? 5 : 6;
	}
	static constexpr u32 shamt(u32 i)  {
		return (i >> 20) & ((u32{1} << shamtLen()) - 1);
	}
	static constexpr u32 shiftType(u32 i) {
		return i >> (20 + shamtLen());
	}
	static constexpr RegType imm(u32 i)    {
		return signExtend<RegType>(i >> 20, 12);
	}
};

template<typename RegType, typename = std::enable_if_t<IsRegType<RegType>>>
struct SType {
	static constexpr u32 opcode(u32 i) { return i & 0x7f;         }
	static constexpr u32 funct3(u32 i) { return (i >> 12) & 0x07; }
	static constexpr u32 rs1(u32 i)    { return (i >> 15) & 0x1f; }
	static constexpr u32 rs2(u32 i)    { return (i >> 20) & 0x1f; }
	static constexpr RegType imm(u32 i)    {
		const RegType raw = ((i >> 25) << 5) | ((i >> 7) & 0x1f);
		return signExtend<RegType>(raw, 12);
	}
};

template<typename RegType, typename = std::enable_if_t<IsRegType<RegType>>>
struct UType {
	static constexpr u32 opcode(u32 i)  { return i & 0x7f;         }
	static constexpr u32 rd(u32 i)      { return (i >> 7) & 0x1f;  }
	static constexpr RegType imm(u32 i) { 
		return signExtend<RegType>(i & 0xfffff000u, 32);
	}
};

}

