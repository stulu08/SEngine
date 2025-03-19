#pragma once
#include "PhysicsHelper.h"

namespace Stulu {
	class STULU_API PhysX;

	class STULU_API PhysicsMaterial {
	public:
		PhysicsMaterial() = default;
		PhysicsMaterial(const PhysicsMaterial&) = default;

		static constexpr float DefaultStaticFriction = 0.6f;
		static constexpr float DefaultDynamicFriction = 0.6f;
		static constexpr float DefaultRestitution = 0.0f;

		float StaticFriction = DefaultStaticFriction;
		float DynamicFriction = DefaultDynamicFriction;
		float Restitution = DefaultRestitution;

		void SetStaticFriction(float value);
		void SetDynamicFriction(float value);
		void SetRestitution(float value);

		void CreateMaterial();
		
		inline physx::PxMaterial* GetMaterial() const {
			return m_material;
		}

	private:
		physx::PxMaterial* m_material = nullptr;
	};
}