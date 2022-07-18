#include "st_pch.h"
#include "RigidbodyComponent.h"
#include "Collider.h"
#include "Stulu/Scene/Components/Components.h"
#include "physx/PxPhysicsAPI.h"

namespace Stulu {
	RigidbodyComponent::RigidbodyComponent(void* body) {
		this->body = body;
	}
	void RigidbodyComponent::destroy() {
		if (body) {
			if(gameObject.hasComponent<BoxColliderComponent>())
				gameObject.getComponent<BoxColliderComponent>().destroy();
			else if (gameObject.hasComponent<SphereColliderComponent>())
				gameObject.getComponent<SphereColliderComponent>().destroy();
			else if (gameObject.hasComponent<CapsuleColliderComponent>())
				gameObject.getComponent<CapsuleColliderComponent>().destroy();
			else if (gameObject.hasComponent<MeshColliderComponent>())
				gameObject.getComponent<MeshColliderComponent>().destroy();

			gameObject.getScene()->getPhysics().getScene()->removeActor(*((physx::PxActor*)body), false);
			((physx::PxActor*)body)->isReleasable() ? ((physx::PxActor*)body)->release() : 0;
			body = nullptr;
		}
	}
	void RigidbodyComponent::addForce(glm::vec3 force, ForceMode mode) {
		if (!body)
			return;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)body;
		physx::PxRigidDynamic* mactor = actor->is<physx::PxRigidDynamic>();
		mactor->addForce(physx::PxVec3(force.x, force.y, force.z), (physx::PxForceMode::Enum)mode);
	}
	void RigidbodyComponent::setPosition(glm::vec3 position) {
		if (!body)
			return;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)body;
		if (kinematic)
			actor->is<physx::PxRigidDynamic>()->setKinematicTarget(physx::PxTransform(PhysicsVec3fromglmVec3(position)));
		else
			actor->is<physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(PhysicsVec3fromglmVec3(position)));
	}
	void RigidbodyComponent::setTransform(glm::vec3 position, glm::quat rotation) {
		if (!body)
			return;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)body;
		if (kinematic)
			actor->is<physx::PxRigidDynamic>()->setKinematicTarget(physx::PxTransform(PhysicsVec3fromglmVec3(position), PhysicsQuatfromglmQuat(rotation)));
		else
			actor->is<physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(PhysicsVec3fromglmVec3(position), PhysicsQuatfromglmQuat(rotation)));
	}
	void RigidbodyComponent::updateFlags() {
		if (!body)
			return;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)body;
		actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, !useGravity);
		physx::PxRigidDynamic* mactor = actor->is<physx::PxRigidDynamic>();
		mactor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, kinematic);
		if (!kinematic) {
			mactor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eRETAIN_ACCELERATIONS, retainAccelaration);
		}
		mactor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !rotationX);
		mactor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, !rotationY);
		mactor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, !rotationZ);
		mactor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, !moveX);
		mactor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, !moveY);
		mactor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, !moveZ);

		
		physx::PxRigidBodyExt::updateMassAndInertia(*mactor, mass, &PhysicsVec3fromglmVec3(massCenterPos));
		actor->is<physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(
			PhysicsVec3fromglmVec3(gameObject.getComponent<TransformComponent>().worldPosition),
			PhysicsQuatfromglmQuat(gameObject.getComponent<TransformComponent>().worldRotation)));
	}
}