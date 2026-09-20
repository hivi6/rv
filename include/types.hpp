#pragma once

#include <concepts>
#include <cstdint>

namespace riscv {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

template <typename T>
concept RegisterType = std::same_as<T, u32> || std::same_as<T, u64>;

template <typename T>
concept BusType = std::same_as<T, u8> || std::same_as<T, u16>
	|| std::same_as<T, u32> || std::same_as<T, u64>;

}

