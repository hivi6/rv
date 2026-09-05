#pragma once

#include "types.hpp"

namespace riscv {

template<RegisterType T>
constexpr T signExtend(T value, u32 bits) {
	const T signBit = T{1} << (bits - 1);
	return (value ^ signBit) - signBit;
}

}

