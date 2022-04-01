#include "st_pch.h"
#include "RigidbodyComponent.h"
#define PX_PHYSX_STATIC_LIB
#include "physx/PxPhysicsAPI.h"

namespace Stulu {
	RigidbodyComponent::RigidbodyComponent(void* body) {
		this->body = body;
	}
	void RigidbodyComponent::destroy(GameObject object, PhysX& physics) {
		if (body) {
			body = nullptr;
		}
	}
	void RigidbodyComponent::addForce(glm::vec3 force, ForceMode mode) {
		physx::PxRigidActor* actor = (physx::PxRigidActor*)body;
		if (rbType == Type::Dynamic) {
			physx::PxRigidDynamic* mactor = actor->is<physx::PxRigidDynamic>();
			mactor->addForce(physx::PxVec3(force.x, force.y, force.z), (physx::PxForceMode::Enum)mode);
		}
		else
			CORE_WARN("Rigidbody is static! Can't add force");
	}
}