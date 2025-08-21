#pragma once
#include <cstdint>
#include <sstream>
#include "Stulu/Core/Core.h"

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

	std::string to_string() {
		return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
	}
};

inline std::ostream& operator<<(std::ostream& os, const Version& e) {
	return os << e.major << "." << e.minor << "." << e.patch;
}

#define ST_ENGINE_VERSION Version(ST_ENGINE_VERSION_MAJOR, ST_ENGINE_VERSION_MINOR, ST_ENGINE_VERSION_PATCH)
#define ST_ENGINE_VERSION_STR (ST_STRINGIFY(ST_ENGINE_VERSION_MAJOR) "." ST_STRINGIFY(ST_ENGINE_VERSION_MINOR) "." ST_STRINGIFY(ST_ENGINE_VERSION_PATCH))
