#include "mailbox.hpp"
#include <fcntl.h>

optional<PerfMailbox> PerfMailbox::initialize() {
	int msr_fd = open("/dev/cpu/0/msr", O_RDWR);
	if (msr_fd < 0)
		return {};
	PerfMailbox p (msr_fd);
	return p;
}

optional<Capabilities> PerfMailbox::getCapabilities(Domain d) {
	const uint8_t command = 0x1;
	uint64_t msr_out = get(d, command, 0).value_or(1ULL << 32);
	if ((uint8_t)(msr_out >> 32) > 0){
		return {};
	}
	return (Capabilities){
		(uint8_t)(msr_out & 0xFF), //max_ratio
		(bool)((msr_out >> 8) & 0x1), //unlocked_ratio
		(bool)((msr_out >> 9) & 0x1), //support_fixed_voltage
		(bool)((msr_out >> 10) & 0x1), //support_offset_voltage
    };
}

optional<TurboSettings> PerfMailbox::getTurboRatios() {
	const uint8_t command = 0x2;
	uint64_t msr_out = get(Domain::Core, command, 0).value_or(1ULL << 32);
	if (!msr_out) {
		return {};
	}
	if ((uint8_t)(msr_out >> 32) > 0) {
		return {};
	}
	TurboSettings turbo_out;
	turbo_out.turbo_ratio[0] = (uint8_t)(msr_out & 0xFF); //single-core turbo
	turbo_out.turbo_ratio[1] = (uint8_t)((msr_out >> 8) & 0xFF); //two-core turbo
	turbo_out.turbo_ratio[2] = (uint8_t)((msr_out >> 16) & 0xFF); // 3-core turbo
	turbo_out.turbo_ratio[3] = (uint8_t)((msr_out >> 24) & 0xFF); // 4-core turbo
	return turbo_out;
}

optional<VoltageSetting> PerfMailbox::getVoltageSettings(Domain d) {
	const uint8_t command = 0x10;
	uint64_t msr_out = get(d, command, 0).value_or(1ULL << 32);
	if ((uint8_t)(msr_out >> 32) > 0) {
		return {};
	}
	VoltageSetting v = {
		(bool)((msr_out >> 20) & 0x1), //use_fixed
		(int)((msr_out >> 21) & 0x7FF), //offset_v
		(int)((msr_out >> 8) & 0xFFF)  //fixed_v
	};
	if (v.offset_v & 0x400) {
		v.offset_v |= 0xFFFFF800;
	}
	return v;
}

bool PerfMailbox::setVoltageSettings(Domain d, VoltageSetting v) {
	const uint8_t command = 0x11;
	uint64_t msr_out = get(d, command, v.getBits()).value_or(1ULL << 32);
	return ((uint8_t)(msr_out >> 32) == 0);
}

optional<uint64_t> PerfMailbox::get(Domain d, uint32_t cmd, uint32_t data) {
	uint64_t msr_in = (((0x1ULL << 31)
	                  | ((int)d << 8)
	                  | cmd) << 32)
	                  | data;
	uint64_t msr_out;
	if (pwrite(msr_fd, &msr_in, sizeof(uint64_t), 0x150) != sizeof(uint64_t))
		return {};
	if (pread(msr_fd, &msr_out, sizeof(uint64_t), 0x150) != sizeof(uint64_t))
		return {};
	return msr_out;
}