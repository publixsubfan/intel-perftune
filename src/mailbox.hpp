#pragma once
#include <cstdint>
#include <unistd.h>
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
	PerfMailbox(PerfMailbox& copy)
		: msr_fd(copy.msr_fd) {
		copy.msr_fd = -1;
	}

	PerfMailbox(PerfMailbox&& move) {
	    this->msr_fd = move.msr_fd;
	    move.msr_fd = -1;
    }

	~PerfMailbox() {
		if (msr_fd != -1) {
			close(msr_fd);
		}
	}

	PerfMailbox& operator = (PerfMailbox&& move) {
	    if (this->msr_fd != -1)
	        close(msr_fd);
	    this->msr_fd = move.msr_fd;
	    move.msr_fd = -1;
    }

	static Result<PerfMailbox, Error> initialize();

	Result<Capabilities, Error> getCapabilities(Domain d);

	Result<TurboSettings, Error> getTurboRatios();

	Result<VoltageSetting, Error> getVoltageSettings(Domain d);

	Error setVoltageSettings(Domain d, VoltageSetting v);

	Result<SVIDSetting, Error> getSVIDSetting();
private:
	PerfMailbox(int fd)
		: msr_fd(fd) { }

	int msr_fd;

	Result<uint64_t, Error> get(Domain d, uint32_t cmd, uint32_t data);
};

}
