#pragma once

#include <cstddef>
#include <expected>
#include <string>
#include <vector>

#include "types.hpp"

namespace riscv {

enum class BusErrorType {
	ADDRESS_OUT_OF_BOUND,
};

struct BusError {
	BusErrorType type;
	std::string msg;

	BusError(BusErrorType t, std::string m): type{t}, msg{m} {}
};

class Bus {
	static constexpr u64 RAM_SIZE = 1024 * 1024; // 1 MiB

public:
	Bus(const std::vector<u8> &bin) : dram{bin}, bSize{bin.size()} {
		dram.resize(RAM_SIZE, u8{0});
	}

	u64 busSize() const {
		return dram.size();
	}

	u64 binSize() const {
		return bSize;
	}

	template <BusType BType>
	std::expected<BType, BusError> load(u64 address) const {
		const auto bytes = sizeof(BType);
		if (address >= busSize()
			|| bytes > busSize() - address) {
			return std::unexpected(BusError(
				BusErrorType::ADDRESS_OUT_OF_BOUND,
				"reading address space is out of bounds"));
		}

		// TODO: little endian for now; handle both, in future
		BType res = 0;
		for (std::size_t i = 0; i < bytes; i++) {
			res |= static_cast<BType>(dram[address + i]) << (8 * i);
		}
		return res;
	}

	template <BusType BType>
	std::expected<void, BusError> store(u64 address, BType value) {
		const auto bytes = sizeof(BType);
		if (address >= busSize()
			|| bytes > busSize() - address) {
			return std::unexpected(BusError(
				BusErrorType::ADDRESS_OUT_OF_BOUND,
				"storage address space is out of bounds"));
		}

		// TODO: little endian for now; handle both, in future
		for (std::size_t i = 0; i < bytes; i++) {
			dram[address + i] = static_cast<u8>(value >> (8 * i));
		}

		return {};
	}

private:
	std::vector<u8> dram;
	u64 bSize;
};

};
