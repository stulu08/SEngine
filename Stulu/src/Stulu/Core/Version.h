#pragma once
#include <memory>

#define ST_MAKE_VERSION(major, minor, patch) ((((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))
struct Version {
	uint32_t major = 1;
	uint32_t minor = 0;
	uint32_t patch = 0;

	Version(uint32_t major = 1, uint32_t minor = 0, uint32_t patch = 0)
		: major(major), minor(minor), patch(patch){}

	operator uint32_t() const {
		return ST_MAKE_VERSION(major, minor, patch);
	}
};

inline std::ostream& operator<<(std::ostream& os, const Version& e) {
	return os << e.major << "." << e.minor << "." << e.patch;
}

#define ST_ENGINE_VERSION Version(1,1,106)
#define ST_ENGINE_NAME "Stulu Engine"