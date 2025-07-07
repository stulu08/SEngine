#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Math/Math.h"

namespace Stulu {
	class UUID {
	public:
		STULU_API UUID();
		STULU_API UUID(const std::string& deterministicValue);
		inline UUID(uint64_t uuid)
			: m_uuid(uuid) {
		}
		inline operator uint64_t() const { return m_uuid; }

		static UUID null;
	private:
		uint64_t m_uuid;
	};

	inline UUID UUID::null = UUID(0);
}

namespace std {
	template<>
	struct hash<Stulu::UUID> {
		std::size_t operator()(const Stulu::UUID& uuid) const {
			return (uint64_t)uuid;
		}
	};
}