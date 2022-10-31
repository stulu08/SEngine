#pragma once
#include "Stulu/Scene/physx/RigidbodyComponent.h"
namespace StuluBindings {
	class GameObject;
}

namespace Stulu {

	class STULU_API Collider : public Component {
	public:
		float staticFriction = ST_DEFAULT_PHYSX_MATERIAL_STATIC_FRICTION;
		float dynamicFriction = ST_DEFAULT_PHYSX_MATERIAL_DYNAMIC_FRICTION;
		float restitution = ST_DEFAULT_PHYSX_MATERIAL_RESTITUTION;
		virtual void destroy() override;
	protected:
		void* shape = nullptr;
		void* rigidbody = nullptr;

		virtual void create(GameObject object, PhysX& physics){}
	};
	class STULU_API BoxColliderComponent : public Collider{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;

		glm::vec3 offset = glm::vec3(0.0f);
		glm::vec3 size = glm::vec3(.5f);
	private:
		virtual void create(GameObject object, PhysX& physics) override;
		friend class StuluBindings::GameObject;
		friend class Scene;
		friend class RigidbodyComponent;
	};
	class STULU_API SphereColliderComponent : public Collider{
	public:
		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;

		glm::vec3 offset = glm::vec3(0.0f);
		float radius = .5f;
	private:
		virtual void create(GameObject object, PhysX& physics) override;
		friend class StuluBindings::GameObject;
		friend class Scene;
		friend class RigidbodyComponent;
	};
	class STULU_API CapsuleColliderComponent : public Collider {
	public:
		CapsuleColliderComponent() = default;
		CapsuleColliderComponent(const CapsuleColliderComponent&) = default;

		glm::vec3 offset = glm::vec3(0.0f);
		float radius = 1.0f;
		float height = 2.0f;
		// if true: height * scale.y; else: height * scale.x;
		bool horizontal = false;
	private:
		virtual void create(GameObject object, PhysX& physics) override;
		friend class StuluBindings::GameObject;
		friend class Scene;
		friend class RigidbodyComponent;
	};
	class STULU_API MeshColliderComponent : public Collider {
	public:
		MeshColliderComponent() = default;
		MeshColliderComponent(const MeshColliderComponent&) = default;

		MeshAsset mesh{ "",nullptr };
		Ref<Mesh> convexMesh = nullptr;
		bool convex = false;
	private:
		virtual void create(GameObject object, PhysX& physics) override;
		friend class StuluBindings::GameObject;
		friend class RigidbodyComponent;
		friend class Scene;
	};
}