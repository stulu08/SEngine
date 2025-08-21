#pragma once
#include "Stulu/Math/Math.h"
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Components/Component.h"
#include "Stulu/Physics/PhysicsScene.h"

namespace Stulu {
	enum class ForceMode {
		Force,
		Impulse,
		VelocityChange,
		Acceleration
	};

	// internal usage only, use for interacting with physx
	class STULU_API RigidActorComponent : public Component {
	public:
		RigidActorComponent() = default;
		RigidActorComponent(const RigidActorComponent&) = default;

		virtual ~RigidActorComponent() = default;

		// checks if actor is valid
		virtual bool RuntimeValid() const = 0;
		// checks if scene has physics enabled and m_actor is valid
		bool RuntimeCanChange() const;

		virtual void Create(PhysicsScene* physics) = 0;
		virtual void Release();

		void ApplyTransformChanges();

		void SetPosition(glm::vec3 position);
		virtual void SetTransform(glm::vec3 position, glm::quat rotation) = 0;
		virtual void WakeUp() const = 0;

		void SetDebugVisuals(bool value) const;

		physx::PxRigidActor* GetActor() const {
			if(RuntimeValid())
				return m_actor;
			return nullptr;
		}
		virtual void onComponentRemove(Registry* registry) override {
			if (!registry->IsScene()) return;
			if (registry->GetAsScene()->PhysicsEnable()) Release();
		};
	protected:
		physx::PxRigidActor* m_actor = nullptr;
		PhysicsScene* m_physics = nullptr;
	};
	class STULU_API RigidStaticComponent : public RigidActorComponent {
	public:
		RigidStaticComponent() = default;
		RigidStaticComponent(const RigidStaticComponent&) = default;
		
		virtual bool RuntimeValid() const override;
		virtual void Create(PhysicsScene* physics) override;

		virtual void SetTransform(glm::vec3 position, glm::quat rotation) override;
		void WakeUp() const override{}

		physx::PxRigidStatic* GetStaticActor() const;
	};
	class STULU_API RigidbodyComponent : public RigidActorComponent {
	public:
		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;

		virtual bool RuntimeValid() const override;
		virtual void Create(PhysicsScene* physics) override;

		virtual void SetTransform(glm::vec3 position, glm::quat rotation) override;
		void AddForce(glm::vec3 force, ForceMode mode) const;
		void WakeUp() const override;

		void SetRotationLock(uint8_t index, bool value);
		void SetMoveLock(uint8_t index, bool value);
		void SetKinematic(bool value);
		void SetRetainAcceleration(bool value);
		void SetUseGravity(bool value);
		void SetMass(float value);
		void SetMassCenterPosition(const glm::vec3& value);

		bool HasRotationLock(uint8_t index) const;
		bool HasMoveLock(uint8_t index) const;
		bool IsKinematic() const;
		bool IsRetainingAcceleration() const;
		bool IsUsingGravity() const;
		float GetMass() const;
		glm::vec3 GetMassCenterPosition() const;

		void ComputeMass(float density);

		physx::PxRigidDynamic* GetDynamicActor() const;

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::Node& node) override;
	private:
		bool UseGravity = true;
		bool RotationLockX = false, RotationLockY = false, RotationLockZ = false;
		bool MoveLockX = false, MoveLockY = false, MoveLockZ = false;
		bool Kinematic = false;
		bool RetainAccelaration = false;
		float Mass = 1.0f;
		glm::vec3 MassCenterPosition = glm::vec3(.0f);

		friend class PhysicsScene;
	};

	template<>
	inline RigidActorComponent& GameObject::getComponent<RigidActorComponent>() const {
		if (hasComponent<RigidbodyComponent>())
			return getComponent<RigidbodyComponent>();
		return getComponent<RigidStaticComponent>();
	}
	template<>
	inline bool GameObject::hasComponent<RigidActorComponent>() const {
		return hasComponent<RigidbodyComponent>() || hasComponent<RigidStaticComponent>();
	}
	template<>
	inline bool GameObject::removeComponent<RigidActorComponent>() const {
		if (hasComponent<RigidbodyComponent>())
			return removeComponent<RigidbodyComponent>();
		return removeComponent<RigidStaticComponent>();
	}
	template<>
	inline RigidActorComponent& GameObject::addComponent<RigidActorComponent>() const {
		CORE_ERROR("Warning dont add a RigidActorComponent to a gameobject, use RigidStaticComponent or RigidbodyComponent");
		if (hasComponent<RigidbodyComponent>())
			return addComponent<RigidbodyComponent>();
		return addComponent<RigidStaticComponent>();
	}
	template<>
	inline RigidActorComponent& GameObject::saveAddComponent<RigidActorComponent>() const {
		CORE_ERROR("Warning dont add a RigidActorComponent to a gameobject, use RigidStaticComponent or RigidbodyComponent");
		if (hasComponent<RigidbodyComponent>())
			return saveAddComponent<RigidbodyComponent>();
		return saveAddComponent<RigidStaticComponent>();
	}
}