#include "st_pch.h"
#include "PhysicsMaterial.h"

#include "PhysX.h"

#define PX_PHYSX_STATIC_LIB
#include "PxConfig.h"
#include "PxPhysicsAPI.h"

namespace Stulu {
	void PhysicsMaterial::SetStaticFriction(float value) {
		if(m_material)
			m_material->setStaticFriction(value);

		StaticFriction = value;
	}
	void PhysicsMaterial::SetDynamicFriction(float value) {
		if (m_material)
			m_material->setDynamicFriction(value);

		DynamicFriction = value;
	}
	void PhysicsMaterial::SetRestitution(float value) {
		if (m_material)
			m_material->setRestitution(value);

		Restitution = value;
	}
	void PhysicsMaterial::CreateMaterial(PhysX* physics) {
		if (physics && physics->getPhysics())
			m_material = physics->getPhysics()->createMaterial(StaticFriction, DynamicFriction, Restitution);
		else
			m_material = nullptr;
	}
}

