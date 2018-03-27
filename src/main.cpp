#include <iostream>
#include <string>
#include "msr.hpp"
#include "mailbox.hpp"

using std::cout;
using std::endl;
using namespace perftune;

Error handleOffsetAdjust(Msr m, Domain d, int mv) {
	PerfMailbox pctrl(m);
	VoltageSetting v;
	v.setOffsetV(mv);
	return pctrl.setVoltageSettings(d, v);
}

Error printCapabilities(Msr m, Domain d) {
	PerfMailbox pctrl(m);
	auto info = pctrl.getCapabilities(d);
	if (!info) {
		return info.getError();
	}
	cout << "Capabilities for ";
	switch (d) {
		case Domain::Core:
			cout << "core";
			break;
		case Domain::Graphics:
			cout << "graphics";
			break;
	}
	cout << " domain:" << endl;
	cout << " - Maximum turbo ratio: " << (int)info->max_ratio << endl
		 << " - Unlocked turbo ratio: "
		 << ((info->unlocked_ratio) ? "Y" : "N") << endl
		 << " - Fixed Voltage control: "
		 << ((info->support_fixed_v) ? "Y" : "N") << endl
		 << " - Offset Voltage control: "
		 << ((info->support_offset_v) ? "Y" : "N") << endl;
	return Error::None;
}

int main(int argc, char** argv) {
	std::string msr_loc("/dev/cpu/0/msr");
	auto msr_dev(Msr::initialize(msr_loc));
	if (!msr_dev) {
		cerr << "Failed to open msr device." << endl
			 << "Check if msr kernel module is loaded, "
			 << "and that you have permissions to read the device." << endl;
		return 0;
	}
	// read domain
	if (argc < 2) {
		cerr << "Must specify a domain." << endl;
		return 0;
	}

	Domain d;
	if (std::string("core").compare(argv[1])) {
		d = Domain::Core;
	} else if (std::string("gfx").compare(argv[1])) {
		d = Domain::Graphics;
	} else {
		cerr << "\"" << argv[1] << "\" is not a valid domain." << endl;
		return 0;
	}

	if (argc < 3) {
		cerr << "Must specify an action." << endl;
		return 0;
	}

	if (std::string("info").compare(argv[2])) {
		//TODO: print more stuff
		printCapabilities(msr_dev.getResult(), d);
	} else if (std::string("voltage").compare(argv[2])) {

	}
	return 0;
}
