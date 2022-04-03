#pragma once
#include "Stulu/Scene/physx/RigidbodyComponent.h"

namespace Stulu {

	class STULU_API Collider : public Component {
	public:
		float staticFriction = .1f;
		float dynamicFriction = .1f;
		float restitution = .0f;
	protected:
		void* shape = nullptr;
		void* rigidbody = nullptr;
		bool localRigidbody = false;//when the rigidbody is created automaticly

		RigidbodyComponent rigidbodyComponent;
		virtual void create(GameObject object, PhysX& physics){}
		virtual void destroy(GameObject object, PhysX& physics);
	};
	class STULU_API BoxColliderComponent : public Collider{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;

		glm::vec3 offset = glm::vec3(0.0f);
		glm::vec3 size = glm::vec3(.5f);
	private:
		virtual void create(GameObject object, PhysX& physics) override;
		friend class Scene;
	};
	class STULU_API SphereColliderComponent : public Collider{
	public:
		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;

		glm::vec3 offset = glm::vec3(0.0f);
		float radius = .5f;
	private:
		virtual void create(GameObject object, PhysX& physics) override;
		friend class Scene;
	};
	class STULU_API MeshColliderComponent : public Collider {
	public:
		MeshColliderComponent() = default;
		MeshColliderComponent(const MeshColliderComponent&) = default;
	private:
		virtual void create(GameObject object, PhysX& physics) override;

		friend class Scene;
	};
}