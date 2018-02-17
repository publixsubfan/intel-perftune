#include <iostream>
#include <string>
#include "msr.hpp"
#include "mailbox.hpp"

using std::cout;
using std::endl;
using namespace perftune;

int main(int argc, char** argv) {
	std::string msr_loc("/dev/cpu/0/msr");
	auto msr_dev(Msr::initialize(msr_loc));
	if (!msr_dev) {
		cerr << "Failed to open msr device." << endl
			 << "Check if msr kernel module is loaded, "
			 << "and that you have permissions to read the device." << endl;
		return 0;
	}

	PerfMailbox pctrl(msr_dev.getResult());

	if (auto capabilities = pctrl.getCapabilities(Domain::Core)) {
		cout << "CPU capabilities:\n"
			 << " - Maximum turbo ratio: " << (int)capabilities->max_ratio << endl
			 << " - Unlocked turbo ratio: "
			 << ((capabilities->unlocked_ratio) ? "Y" : "N") << endl
			 << " - Fixed Voltage control: "
			 << ((capabilities->support_fixed_v) ? "Y" : "N") << endl
			 << " - Offset Voltage control: "
			 << ((capabilities->support_offset_v) ? "Y" : "N") << endl;
	}

	if (auto capabilities = pctrl.getCapabilities(Domain::Graphics)) {
		cout << "GPU capabilities:\n"
			 << " - Maximum clock ratio: " << (int)capabilities->max_ratio << endl
			 << " - Unlocked clock ratio: "
			 << ((capabilities->unlocked_ratio) ? "Y" : "N") << endl
			 << " - Fixed Voltage control: "
			 << ((capabilities->support_fixed_v) ? "Y" : "N") << endl
			 << " - Offset Voltage control: "
			 << ((capabilities->support_offset_v) ? "Y" : "N") << endl;
	}
	return 0;
}
