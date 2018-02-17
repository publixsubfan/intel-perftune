#pragma once
#include <cstdint>
#include "msr.hpp"
#include "result.hpp"

using namespace std;

namespace perftune
{

using util::Result;

struct Capabilities {
	uint8_t max_ratio;
	bool unlocked_ratio;
	bool support_fixed_v;
	bool support_offset_v;
};

struct VoltageSetting {
	bool use_fixed;
	int offset_v;
	int fixed_v;

	uint32_t getBits() {
		return (((fixed_v & 0xFFF) << 8)
				|((use_fixed ? 0x1 : 0x0) << 20)
				|(offset_v & 0x7FF) << 21);
	}
};

struct TurboSettings {
	uint8_t turbo_ratio[4];
};

struct SVIDSetting {
	bool disable_svid;
	int vcc_in;

	bool isDynamic() {
		return vcc_in == 0;
	}
};

enum class Domain {
	Core = 0,
	Graphics = 1,
	Cache = 2,
	SysAgent = 3,
	AnalogIO = 4,
	DigitalIO = 5
};

enum class Error {
	None,
	IoError,
	MailboxError
};

class PerfMailbox {
public:
	PerfMailbox(Msr msr)
		: _msr(msr) { }

	Result<Capabilities, Error> getCapabilities(Domain d);

	Result<TurboSettings, Error> getTurboRatios();

	Result<VoltageSetting, Error> getVoltageSettings(Domain d);

	Error setVoltageSettings(Domain d, VoltageSetting v);

	Result<SVIDSetting, Error> getSVIDSetting();
private:
	Msr _msr;
	const int msr_offset = 0x150;

	Result<uint64_t, Error> get(Domain d, uint32_t cmd, uint32_t data);
};

}
