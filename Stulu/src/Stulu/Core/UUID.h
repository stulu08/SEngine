#pragma once
#include "Stulu/Core/Core.h"

namespace Stulu {
	class UUID {
	public:
		UUID();
		UUID(uint64_t uuid);
		operator uint64_t() const { return m_uuid; }
	private:
		uint64_t m_uuid;
	};
}

namespace std {
	template<>
	struct hash<Stulu::UUID> {
		std::size_t operator()(const Stulu::UUID& uuid) const {
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}