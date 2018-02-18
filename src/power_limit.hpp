#pragma once
#include "msr.hpp"
#include "result.hpp"

namespace perftune
{

using util::Result;

struct PowerLimitSetting {
	int time;
	bool clamp_limit;
	bool enable;
	uint16_t pwr_max;
}

enum class PowerDomain {
	// long-term package power limit
	PackagePL1,
	// short-term package power limit
	PackagePL2,
	// CPU core power limit
	PP0,
	// uncore power limit
	PP1
}

class PowerLimitMsr {
public:
	PowerLimitMsr(Msr msr) : _msr(msr) { }

	Result<PowerLimitSetting, Error> getPowerLimit(PowerDomain p);

	Result<bool, Error> isLocked(PowerDomain p);

	Error setPowerLimit(PowerDomain p, PowerLimitSetting l);

private:
	Msr _msr;
	const int pwr_unit_offset = 0x606;
	const int pkg_pwr_offset = 0x610;
	const int pp0_pwr_offset = 0x638;
	const int pp1_pwr_offset = 0x640;

	bool to_msr_fmt(PowerLimitSetting& l, bool is_pkg);
	bool to_user_fmt(PowerLimitSetting& l, bool is_pkg);
};

}