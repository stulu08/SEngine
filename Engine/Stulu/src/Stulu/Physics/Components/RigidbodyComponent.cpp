#include "st_pch.h"
#include "RigidbodyComponent.h"


#include "Collider.h"
#include "Stulu/Physics/PhysicsScene.h"
#include "Stulu/Physics/PhysicsModule.h"
#include "Stulu/Scene/Components/Components.h"

#define PX_PHYSX_STATIC_LIB
#include "PxPhysicsAPI.h"

namespace Stulu {
	inline bool RigidActorComponent::RuntimeCanChange() const {
		return RuntimeValid() && m_physics->IsValid();
	}
    void RigidActorComponent::Release() {
        if (!gameObject) {
            m_actor = nullptr;
            return;
        }

        if (gameObject.hasComponent<BoxColliderComponent>())
            gameObject.getComponent<BoxColliderComponent>().Release();
        else if (gameObject.hasComponent<SphereColliderComponent>())
            gameObject.getComponent<SphereColliderComponent>().Release();
        else if (gameObject.hasComponent<CapsuleColliderComponent>())
            gameObject.getComponent<CapsuleColliderComponent>().Release();
        else if (gameObject.hasComponent<MeshColliderComponent>())
            gameObject.getComponent<MeshColliderComponent>().Release();

        if (m_physics->IsValid())
            m_physics->GetPhysicsScene()->removeActor(*m_actor, false);

        if (m_actor->isReleasable())
            m_actor->release();
        m_actor = nullptr;
    }
    void RigidActorComponent::ApplyTransformChanges() {
        if (!RuntimeCanChange())
            return;

        auto& tc = gameObject.getComponent<TransformComponent>();

        if (!tc.IsUpdatingPhysics()) {
            return;
        }

        physx::PxTransform tr = m_actor->getGlobalPose();

        const glm::vec3 pos = PhysicsHelper::PhysXToVec3(tr.p);
        const glm::quat rot = PhysicsHelper::PhysXToQuat(tr.q);

        const glm::vec3 worldPos = tc.GetWorldPosition();
        const glm::quat worldRot = tc.GetWorldRotation();

        tc.SetUpdatePhysics(false);

        // Use epsilon comparison to avoid floating-point inaccuracies

        if (!glm::all(glm::epsilonEqual(pos, worldPos, 0.0001f))) {
            tc.SetWorldPosition(pos);
        }
        if (!glm::all(glm::epsilonEqual(rot, worldRot, 0.0001f))) {
            tc.SetWorldRotation(rot);
        }

        tc.SetUpdatePhysics(true);
    }
    void RigidActorComponent::SetPosition(glm::vec3 position) {
        if (!RuntimeCanChange())
            return;
        SetTransform(position, PhysicsHelper::PhysXToQuat(m_actor->getGlobalPose().q));
    }

    void RigidActorComponent::SetDebugVisuals(bool value) const {
        if (!RuntimeCanChange())
            return;

        m_actor->setActorFlag(physx::PxActorFlag::eVISUALIZATION, value);
    }

	inline bool RigidStaticComponent::RuntimeValid() const {
		return m_actor && m_actor->is<physx::PxRigidStatic>();
	}
	void RigidStaticComponent::Create(PhysicsScene* physics) {
		const auto& transform = gameObject.getComponent<TransformComponent>();
		m_actor = PhysicsModule::Get().GetPhysics()->createRigidStatic(physx::PxTransform(
            PhysicsHelper::Vec3ToPhysX(transform.GetWorldPosition()), 
            PhysicsHelper::QuatToPhysX(transform.GetWorldRotation())));

        physics->GetPhysicsScene()->addActor(*m_actor);
        m_physics = physics;
	}
	void RigidStaticComponent::SetTransform(glm::vec3 position, glm::quat rotation) {
		if (!RuntimeCanChange())
			return;
		m_actor->setGlobalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(position), PhysicsHelper::QuatToPhysX(rotation)));
	}
	inline physx::PxRigidStatic* RigidStaticComponent::GetStaticActor() const {
		return m_actor->is<physx::PxRigidStatic>();
	}



	inline bool RigidbodyComponent::RuntimeValid() const {
		return m_actor && m_actor->is<physx::PxRigidDynamic>();
	}
	void RigidbodyComponent::Create(PhysicsScene* physics) {
        const auto& transform = gameObject.getComponent<TransformComponent>();
        m_actor = PhysicsModule::Get().GetPhysics()->createRigidDynamic(physx::PxTransform(
            PhysicsHelper::Vec3ToPhysX(transform.GetWorldPosition()),
            PhysicsHelper::QuatToPhysX(transform.GetWorldRotation())));

        physics->GetPhysicsScene()->addActor(*m_actor);
        m_physics = physics;
    }
	void RigidbodyComponent::SetTransform(glm::vec3 position, glm::quat rotation) {
		if (!RuntimeCanChange())
			return;

		physx::PxRigidDynamic* actor = GetDynamicActor();
		physx::PxQuat quat = PhysicsHelper::QuatToPhysX(rotation);
		physx::PxVec3 pos = PhysicsHelper::Vec3ToPhysX(position);

		if (actor->getRigidBodyFlags() & physx::PxRigidBodyFlag::Enum::eKINEMATIC) {
			actor->setKinematicTarget(physx::PxTransform(pos, quat));
		}
		else {
			actor->setGlobalPose(physx::PxTransform(pos, quat));
			actor->setLinearVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f), false);
			actor->setAngularVelocity(physx::PxVec3(0.0f, 0.0f, 0.0f), false);
		}
	}
    void RigidbodyComponent::WakeUp() const {
        if (!RuntimeCanChange())
            return;
        physx::PxRigidDynamic* actor = GetDynamicActor();
        if (actor->isSleeping())
            actor->wakeUp();
    }
	void RigidbodyComponent::AddForce(glm::vec3 force, ForceMode mode) const {
		if (!RuntimeCanChange())
			return;

		physx::PxRigidDynamic* actor = GetDynamicActor();
		actor->addForce(physx::PxVec3(force.x, force.y, force.z), (physx::PxForceMode::Enum)mode);
	}
	inline physx::PxRigidDynamic* RigidbodyComponent::GetDynamicActor() const {
		return m_actor->is<physx::PxRigidDynamic>();
	}

    void RigidbodyComponent::SetRotationLock(uint8_t index, bool value) {
        if (!RuntimeCanChange()) {
            return;
        }

        physx::PxRigidDynamic* actor = GetDynamicActor();
        if (!actor) {
            return;
        }
        
        physx::PxRigidDynamicLockFlags flags = actor->getRigidDynamicLockFlags();

        switch (index) {
        case 0: RotationX = value; flags = value ? flags | physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X : flags & ~physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X; break;
        case 1: RotationY = value; flags = value ? flags | physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y : flags & ~physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y; break;
        case 2: RotationZ = value; flags = value ? flags | physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z : flags & ~physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z; break;
        default: return;
        }
        actor->setRigidDynamicLockFlags(flags);
    }

    void RigidbodyComponent::SetMoveLock(uint8_t index, bool value) {
        if (!RuntimeCanChange()) {
            return;
        }

        physx::PxRigidDynamic* actor = GetDynamicActor();
        if (!actor) {
            return;
        }

        physx::PxRigidDynamicLockFlags flags = actor->getRigidDynamicLockFlags();

        switch (index) {
        case 0: MoveX = value; flags = value ? flags | physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X : flags & ~physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X; break;
        case 1: MoveY = value; flags = value ? flags | physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y : flags & ~physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y; break;
        case 2: MoveZ = value; flags = value ? flags | physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z : flags & ~physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z; break;
        default: return;
        }

        actor->setRigidDynamicLockFlags(flags);
    }

    void RigidbodyComponent::SetKinematic(bool value) {
        if (!RuntimeCanChange()) {
            return;
        }
        Kinematic = value;
        physx::PxRigidDynamic* actor = GetDynamicActor();
        actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, value);
    }

    void RigidbodyComponent::SetRetainAcceleration(bool value) {
        if (!RuntimeCanChange()) {
            return;
        }
        
        RetainAccelaration = value;
        if (!Kinematic) {
            physx::PxRigidDynamic* actor = GetDynamicActor();
            actor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eRETAIN_ACCELERATIONS, value);
        }
    }
    void RigidbodyComponent::SetUseGravity(bool value) {
        if (!RuntimeCanChange()) {
            return;
        }
        UseGravity = value;
        physx::PxRigidDynamic* actor = GetDynamicActor();
        actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !value);
    }
    void RigidbodyComponent::SetMass(float value) {
        if (!RuntimeCanChange()) {
            return;
        }
        if (value <= 0.0f) {
            return;
        }
        Mass = value;
        physx::PxRigidDynamic* actor = GetDynamicActor();
        actor->setMass(value);
    }
    void RigidbodyComponent::SetMassCenterPosition(const glm::vec3& value) {
        if (!RuntimeCanChange()) {
            return;
        }
        MassCenterPosition = value;
        physx::PxRigidDynamic* actor = GetDynamicActor();
        actor->setCMassLocalPose(physx::PxTransform(PhysicsHelper::Vec3ToPhysX(value)));
    }
    bool RigidbodyComponent::HasRotationLock(uint8_t index) const {
        switch (index) {
        case 0: return RotationX;
        case 1: return RotationY;
        case 2: return RotationZ;
        default: return false;
        }
    }
    bool RigidbodyComponent::HasMoveLock(uint8_t index) const {
        switch (index) {
        case 0: return MoveX;
        case 1: return MoveY;
        case 2: return MoveZ;
        default: return false;
        }
    }
    bool RigidbodyComponent::IsKinematic() const {
        return Kinematic;
    }
    bool RigidbodyComponent::IsRetainingAcceleration() const {
        return RetainAccelaration;
    }
    bool RigidbodyComponent::IsUsingGravity() const {
        return UseGravity;
    }
    float RigidbodyComponent::GetMass() const {
        return Mass;
    }
    glm::vec3 RigidbodyComponent::GetMassCenterPosition() const {
        return MassCenterPosition;
    }

    void RigidbodyComponent::ComputeMass(float density) {
        if (!RuntimeCanChange()) {
            return;
        }
        physx::PxRigidDynamic* actor = GetDynamicActor();
        physx::PxRigidBodyExt::updateMassAndInertia(*actor, density, &PhysicsHelper::Vec3ToPhysX(MassCenterPosition));

        Mass = actor->getMass();
    }

}