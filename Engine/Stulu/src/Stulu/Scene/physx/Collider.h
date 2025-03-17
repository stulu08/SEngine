#pragma once
#include "RigidbodyComponent.h"
#include "PhysicsMaterial.h"

#include "Stulu/Renderer/Mesh.h"

namespace StuluBindings {
	class GameObject;
}

namespace Stulu {

	class STULU_API Collider : public Component {
	public:
		virtual ~Collider() = default;

		void Release();

		PhysicsMaterial PhysicsMaterial;

		// Checks if physics are enable, the shape is valid and an actor is attached
		inline bool RuntimeCanChange() const {
			return gameObject.getScene()->PhysicsEnable() && m_shape && HasActorAttached();
		}

		physx::PxShape* GetShape() const {
			return m_shape;
		}
		bool HasActorAttached() const;
		RigidActorComponent& GetActor() const;

		RigidbodyComponent& GetRigidBody() const {
			return gameObject.getComponent<RigidbodyComponent>();
		}
		inline bool HasRigidBody() const {
			return gameObject.hasComponent<RigidbodyComponent>();
		}

		virtual void onComponentAdded(Scene* scene) override {
			if (scene->PhysicsEnable())
				this->Create();
		};
		virtual void onComponentRemove(Scene* scene) override {
			if (scene->PhysicsEnable())
				Release();
		};
		
		void SetDebugVisuals(bool value) const;
	protected:
		physx::PxShape* m_shape = nullptr;
		
		inline physx::PxPhysics* GetPhysics() const {
			return gameObject.getScene()->getPhysics()->getPhysics();
		}

		void CreateActor();
		virtual void Create() = 0;
	};
	class STULU_API BoxColliderComponent : public Collider{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;

		virtual void Create();

		glm::vec3 GetOffset() const { return Offset; }
		glm::vec3 GetSize() const { return Size; }

		void SetOffset(glm::vec3 value) { Offset = value; }
		void SetSize(glm::vec3 value) { Size = value; }
	private:
		friend class SceneSerializer;
		glm::vec3 Offset = glm::vec3(0.0f);
		glm::vec3 Size = glm::vec3(.5f);

	};
	class STULU_API SphereColliderComponent : public Collider{
	public:
		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;

		virtual void Create();

		glm::vec3 GetOffset() const { return Offset; }
		float GetRadius() const { return Radius; }

		void SetOffset(glm::vec3 value) { Offset = value; }
		void SetRadius(float value) { Radius = value; }
	private:
		friend class SceneSerializer;
		glm::vec3 Offset = glm::vec3(0.0f);
		float Radius = .5f;

	};
	class STULU_API CapsuleColliderComponent : public Collider {
	public:
		CapsuleColliderComponent() = default;
		CapsuleColliderComponent(const CapsuleColliderComponent&) = default;

		virtual void Create();

		glm::vec3 GetOffset() const { return Offset; }
		float GetRadius() const { return Radius; }
		float GetHeight() const { return Height; }
		bool GetHorizontal() const { return Horizontal; }

		void SetOffset(glm::vec3 value) { Offset = value; }
		void SetRadius(float value) { Radius = value; }
		void SetHeight(float value) { Height = value; }
		void SetHorizontal(bool value) { Horizontal = value; }
	private:
		friend class SceneSerializer;

		glm::vec3 Offset = glm::vec3(0.0f);
		float Radius = 1.0f;
		float Height = 2.0f;
		// if true: height * scale.y; else: height * scale.x;
		bool Horizontal = false;
	};
	class STULU_API MeshColliderComponent : public Collider {
	public:
		MeshColliderComponent() = default;
		MeshColliderComponent(const MeshColliderComponent&) = default;

		virtual void Create();

		void BuildConvex();

		const MeshAsset& GetMesh() const {
			return Mesh;
		}
		const Ref<Mesh> GetConvexMesh() const {
			return ConvexMesh;
		}
		bool GetConvex() const {
			return Convex;
		}

		void SetMesh(const MeshAsset& asset) {
			Mesh = asset;
		}
		void SetConvexMesh(const Ref<Stulu::Mesh>& mesh) {
			ConvexMesh = mesh;
		}
		void SetConvex(bool value) {
			Convex = value;
		}
	private:
		friend class SceneSerializer;
		MeshAsset Mesh = { "",nullptr };
		Ref<Stulu::Mesh> ConvexMesh = nullptr;
		bool Convex = false;
	};
}