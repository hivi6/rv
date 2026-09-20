#pragma once

#include <iomanip>
#include <sstream>

#include "types.hpp"

namespace riscv {

template<RegisterType T>
constexpr T signExtend(T value, u32 bits) {
	const T signBit = T{1} << (bits - 1);
	return (value ^ signBit) - signBit;
}

template<RegisterType T>
std::string toHex(T value) {
	std::ostringstream out;

	out << "0x"
	    << std::hex
	    << std::setw(sizeof(T) * 2)
	    << std::setfill('0')
	    << value;

	return out.str();
}

template<RegisterType T>
constexpr u32 xlen() {
	return sizeof(T) * 8;
}

}

