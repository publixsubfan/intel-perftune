#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "result.hpp"

using namespace std;

namespace perftune
{

using util::Result;

class Msr {
public:
	static Result<Msr, int> initialize(std::string& loc) {
		int msr_fd = open(loc.c_str(), O_RDWR);
		if (msr_fd < 0)
			return errno;
		Msr ret(msr_fd);
		return ret;
	}

	inline int read_msr(int offset, uint64_t& data) {
		if (pread(_fh->msr_fd, &data, sizeof(uint64_t), offset) != sizeof(uint64_t))
			return errno;
		return 0;
	}

	inline int write_msr(int offset, uint64_t data) {
		if (pwrite(_fh->msr_fd, &data, sizeof(uint64_t), offset) != sizeof(uint64_t))
			return errno;
		return 0;
	}
private:
	struct _Handle {
		_Handle(int fd) : msr_fd(fd) { }
		~_Handle() {
			if (msr_fd != -1)
				close(msr_fd);
		}
		int msr_fd = -1;
	};

	std::shared_ptr<_Handle> _fh;
	Msr(int fd) : _fh(std::make_shared<_Handle>(fd)) { }
};

}