#pragma once
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <variant>

using namespace std;

struct Capabilities {
    uint8_t max_ratio;
    bool unlocked_ratio;
    bool support_fixed_voltage;
    bool support_offset_voltage;
};

struct VoltageSettings {
    bool use_fixed;
    int offset_v;
    int fixed_v;

    uint32_t getBits() {
        return (((fixed_v & 0xFFF) << 8)
                |((use_fixed ? 0x1 : 0x0) << 20)
                |(offset_v & 0x7FF) << 21);
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
    bool initialize() {
        msr_fd = open("/dev/cpu/0/msr", O_RDWR);
        if (msr_fd < 0) {
            cerr << "Unable to open MSR for reading." << endl;
            cerr << "Check if the msr module is loaded, or if you have permission to access the msr device." << endl;
            return false;
        }
        return true;
    }

    bool getCapabilities(Domain d, Capabilities& c) {
        const uint8_t command = 0x1;
        uint64_t msr_out = get(d, command, 0);
        if ((uint8_t)(msr_out >> 32) > 0) {
            //an error occured
            return false;
        }
        c = {
            (uint8_t)(msr_out & 0xFF), //max_ratio
            (bool)((msr_out >> 8) & 0x1), //unlocked_ratio
            (bool)((msr_out >> 9) & 0x1), //support_fixed_voltage
            (bool)((msr_out >> 10) & 0x1), //support_offset_voltage
        };
        return true;
    }

    bool getTurboRatios(uint8_t (&turbos)[4]) {
        const uint8_t command = 0x2;
        uint64_t msr_out = get(Domain::Core, command, 0);
        if ((uint8_t)(msr_out >> 32) > 0) {
            return false;
        }
        turbos[0] = (uint8_t)(msr_out & 0xFF); //single-core turbo
        turbos[1] = (uint8_t)((msr_out >> 8) & 0xFF); //two-core turbo
        turbos[2] = (uint8_t)((msr_out >> 16) & 0xFF); // 3-core turbo
        turbos[3] = (uint8_t)((msr_out >> 24) & 0xFF); // 4-core turbo
        return true;
    }

    bool getVoltageSettings(Domain d, VoltageSettings& v) {
        const uint8_t command = 0x10;
        uint64_t msr_out = get(d, command, 0);
        if ((uint8_t)(msr_out >> 32) > 0) {
            return false;
        }
        v = {
            (bool)((msr_out >> 20) & 0x1), //use_fixed
            (int)((msr_out >> 21) & 0x7FF), //offset_v
            (int)((msr_out >> 8) & 0xFFF)  //fixed_v
        };
        if (v.offset_v & 0x400) {
            v.offset_v |= 0xFFFFF800;
        }
        return true;
    }

    bool setVoltageSettings(Domain d, VoltageSettings& v) {
        const uint8_t command = 0x11;
        uint64_t msr_out = get(d, command, v.getBits());
        if ((uint8_t)(msr_out >> 32) > 0) {
            return false;
        }
        return true;
    }

    PerfMailbox()
        : msr_fd(-1) { }

    ~PerfMailbox() {
        if (msr_fd >= 0) {
            close(msr_fd);
        }
    }
private:
    int msr_fd;

    uint64_t get(Domain d, uint32_t cmd, uint32_t data) {
        uint64_t msr_in = (((0x1ULL << 31)
                            | ((int)d << 8)
                            | cmd) << 32) | data;
        uint64_t msr_out;
        if (pwrite(msr_fd, &msr_in, sizeof(uint64_t), 0x150) != sizeof(uint64_t)) {
            cerr << "Unable to write to power tuning mailbox" << endl;
            msr_out = 1ULL << 32;
        }
        if (pread(msr_fd, &msr_out, sizeof(uint64_t), 0x150) != sizeof(uint64_t)) {
            cerr << "Unable to read result of command" << endl;
            msr_out = 1ULL << 32;
        }
        return msr_out;
    }
};
