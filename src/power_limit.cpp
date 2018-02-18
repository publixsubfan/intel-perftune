#include "power_limit.hpp"

namespace perftune
{

using util::Result;


Result<PowerLimitSetting, Error> PowerLimitMsr::getPowerLimit(PowerDomain p);

Result<bool, Error> PowerLimitMsr::isLocked(PowerDomain p);

Error PowerLimitMsr::setPowerLimit(PowerDomain p, PowerLimitSetting l);

bool PowerLimitMsr::to_msr_fmt(PowerLimitSetting& l, bool is_pkg) {
	uint64_t power_units;
	if (_msr.read_msr(pwr_unit_offset, power_units) != 0)
		return false;
	int time_units = (power_units >> 16) & 0x0F;
	int pwr_units = power_units & 0x0F;
	
}

bool PowerLimitMsr::to_user_fmt(PowerLimitSetting& l, bool is_pkg) {

}

}