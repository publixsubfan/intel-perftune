#pragma once
#include <optional>
#include <unistd.h>

using namespace std;

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

class PerfMailbox {
public:

	PerfMailbox(PerfMailbox& copy)
		: msr_fd(copy.msr_fd) { }

	~PerfMailbox() {
		if (msr_fd >= 0) {
			close(msr_fd);
		}
	}

	static optional<PerfMailbox> initialize();

	optional<Capabilities> getCapabilities(Domain d);

	optional<TurboSettings> getTurboRatios();

	optional<VoltageSetting> getVoltageSettings(Domain d);

	bool setVoltageSettings(Domain d, VoltageSetting v);

	optional<SVIDSetting> getSVIDSetting();
private:
	PerfMailbox(int fd)
		: msr_fd(fd) { }

	int msr_fd;

	optional<uint64_t> get(Domain d, uint32_t cmd, uint32_t data);
};
