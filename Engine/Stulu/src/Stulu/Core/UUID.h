#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Math/Math.h"

namespace Stulu {
	class STULU_API UUID {
	public:
		UUID();
		UUID(uint64_t uuid);
		operator uint64_t() const { return m_uuid; }

		static UUID null;
	private:
		uint64_t m_uuid;
	};
}

namespace std {
	template<>
	struct hash<Stulu::UUID> {
		std::size_t operator()(const Stulu::UUID& uuid) const {
			return (uint64_t)uuid;
		}
	};
}