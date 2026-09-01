#pragma once

#include "types.hpp"

namespace riscv {

template<typename T>
constexpr bool IsRegType = 
	(std::is_same<T, u32>::value || std::is_same<T, u64>::value);

template<typename RegType, typename = std::enable_if_t<IsRegType<RegType>>>
constexpr RegType signExtend(RegType value, u32 bits) {
	const RegType signBit = RegType{1} << (bits - 1);
	return (value ^ signBit) - signBit;
}

}

