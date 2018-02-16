#include <iostream>
#include "mailbox.hpp"

using std::cout;
using std::endl;
using namespace perftune;

int main(int argc, char** argv) {
	auto pctrl(PerfMailbox::initialize());
	if (!pctrl) {
		cerr << "Failed to open msr device." << endl
			 << "Check if msr kernel module is loaded,"
			 << "and that you have permissions to read the device." << endl;
		return 0;
	}

	if (auto capabilities = pctrl->getCapabilities(Domain::Core)) {
		cout << "CPU capabilities:\n"
			 << " - Maximum turbo ratio: " << capabilities->max_ratio << endl
			 << " - Unlocked turbo ratio: "
			 << ((capabilities->unlocked_ratio) ? "Y" : "N") << endl
			 << " - Fixed Voltage control: "
			 << ((capabilities->support_fixed_v) ? "Y" : "N") << endl
			 << " - Offset Voltage control: "
			 << ((capabilities->support_offset_v) ? "Y" : "N") << endl;
	}

	if (auto capabilities = pctrl->getCapabilities(Domain::Graphics)) {
		cout << "CPU capabilities:\n"
			 << " - Maximum turbo ratio: " << capabilities->max_ratio << endl
			 << " - Unlocked turbo ratio: "
			 << ((capabilities->unlocked_ratio) ? "Y" : "N") << endl
			 << " - Fixed Voltage control: "
			 << ((capabilities->support_fixed_v) ? "Y" : "N") << endl
			 << " - Offset Voltage control: "
			 << ((capabilities->support_offset_v) ? "Y" : "N") << endl;
	}
	return 0;
}