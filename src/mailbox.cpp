#include "mailbox.hpp"
#include <fcntl.h>

namespace perftune
{

using util::Result;
using util::Error;

Result<Capabilities> PerfMailbox::getCapabilities(Domain d) {
	const uint8_t command = 0x1;
	auto msr_out = get(d, command, 0);
	if (!msr_out) {
		return msr_out.getError();
	}
	uint64_t val = msr_out.getResult();
	if ((uint8_t)(val >> 32) > 0){
		return Error::MailboxError;
	}
	return (Capabilities) {
		(uint8_t)(val & 0xFF), //max_ratio
		(bool)((val >> 8) & 0x1), //unlocked_ratio
		(bool)((val >> 9) & 0x1), //support_fixed_voltage
		(bool)((val >> 10) & 0x1), //support_offset_voltage
    };
}

Result<TurboSettings> PerfMailbox::getTurboRatios() {
	const uint8_t command = 0x2;
	auto msr_out = get(Domain::Core, command, 0);
	if (!msr_out) {
		return msr_out.getError();
	}
	uint64_t val = msr_out.getResult();
	if ((uint8_t)(val >> 32) > 0) {
		return Error::MailboxError;
	}
	TurboSettings turbo_out;
	turbo_out.turbo_ratio[0] = (uint8_t)(val & 0xFF); //single-core turbo
	turbo_out.turbo_ratio[1] = (uint8_t)((val >> 8) & 0xFF); //two-core turbo
	turbo_out.turbo_ratio[2] = (uint8_t)((val >> 16) & 0xFF); // 3-core turbo
	turbo_out.turbo_ratio[3] = (uint8_t)((val >> 24) & 0xFF); // 4-core turbo
	return turbo_out;
}

Result<VoltageSetting> PerfMailbox::getVoltageSettings(Domain d) {
	const uint8_t command = 0x10;
	auto msr_out = get(d, command, 0);
	if (!msr_out) {
		return msr_out.getError();
	}
	uint64_t val = msr_out.getResult();
	if ((uint8_t)(val >> 32) > 0) {
		return Error::MailboxError;
	}
	VoltageSetting v = {
		(bool)((val >> 20) & 0x1), //use_fixed
		(int)((val >> 21) & 0x7FF), //offset_v
		(int)((val >> 8) & 0xFFF)  //fixed_v
	};
	if (v.offset_v & 0x400) {
		v.offset_v |= 0xFFFFF800;
	}
	return v;
}

Error PerfMailbox::setVoltageSettings(Domain d, VoltageSetting v) {
	const uint8_t command = 0x11;
	auto msr_out = get(d, command, v.getBits());
	if (!msr_out) {
		return msr_out.getError();
	}
	uint64_t val = msr_out.getResult();
	if ((uint8_t)(val >> 32) > 0) {
		return Error::MailboxError;
	}
	return Error::None;
}

Result<SVIDSetting> PerfMailbox::getSVIDSetting() {
    const uint8_t command = 0x12;
    auto msr_out = get(Domain::Core, command, 0);
	if (!msr_out) {
		return msr_out.getError();
	}
	uint64_t val = msr_out.getResult();
	if ((uint8_t)(val >> 32) > 0) {
		return Error::MailboxError;
	}
    SVIDSetting sv = {
        (bool)((val >> 31) & 0x1), //disable_svid
        (int)(val & 0x7FF)
    };
    return sv;
}

Result<uint64_t> PerfMailbox::get(Domain d, uint32_t cmd, uint32_t data) {
	uint64_t msr_in = (((0x1ULL << 31)
	                  | ((int)d << 8)
	                  | cmd) << 32)
	                  | data;
	uint64_t msr_out;
	if (_msr.write_msr(msr_offset, msr_in) != 0)
		return Error::IoError;
	if (_msr.read_msr(msr_offset, msr_out) != 0)
		return Error::IoError;
	return msr_out;
}

}