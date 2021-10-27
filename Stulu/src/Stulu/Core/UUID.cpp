#include "st_pch.h"
#include "UUID.h"

#include "Stulu/Math/Math.h"

namespace Stulu {

	static std::random_device s_randomDevice;
	static std::mt19937_64 s_randomeEngine(s_randomDevice());
	static std::uniform_int_distribution<uint64_t> s_uniformDistribution;

	UUID::UUID()
		: m_uuid(s_uniformDistribution(s_randomeEngine)) {
		
	}

	UUID::UUID(uint64_t uuid)
		: m_uuid(uuid) {
	}

}