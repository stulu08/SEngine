#pragma once
#include "RigidbodyComponent.h"
#include "Stulu/Physics/PhysicsMaterial.h"
#include "Stulu/Physics/PhysicsScene.h"

#include "Stulu/Renderer/Mesh.h"
#include "Stulu/Scripting/EventCaller.h"

namespace StuluBindings {
	class GameObject;
}

namespace Stulu {

	class STULU_API Collider : public Component {
	public:
		virtual ~Collider() = default;

		void Release();

		PhysicsMaterial material;
		void SetMaterial(PhysicsMaterial& newMaterial);

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

		virtual void onComponentAdded(Registry* registry) override {
			if (!registry->IsScene()) return;
			const Scene* scene = registry->GetAsScene();

			if (scene->PhysicsEnable() && scene->getCaller()->HasLayer<PhysicsScene>()) {
				PhysicsScene* physicsLayer = &scene->getCaller()->GetLayer<PhysicsScene>();
				if (physicsLayer->IsValid()) {
					this->Create(&scene->getCaller()->GetLayer<PhysicsScene>());
				}
			}
		};
		virtual void onComponentRemove(Registry* scene) override {
			if (m_physics && m_physics->IsValid())
				Release();
		};
		
		void SetDebugVisuals(bool value) const;
	protected:
		physx::PxShape* m_shape = nullptr;
		PhysicsScene* m_physics = nullptr;
		
		void CreateActor();
		virtual void Create(PhysicsScene* physics) = 0;
	};
	class STULU_API BoxColliderComponent : public Collider{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;

		virtual void Create(PhysicsScene* physics);

		glm::vec3 GetOffset() const { return Offset; }
		glm::vec3 GetSize() const { return Size; }

		void SetOffset(glm::vec3 value);
		void SetSize(glm::vec3 value);

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "Offset" << YAML::Value << Offset; 
			out << YAML::Key << "Size" << YAML::Value << Size;
			out << YAML::Key << "material" << YAML::Value << material;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["Offset"])
				Offset = node["Offset"].as<glm::vec3>();
			if (node["Size"])
				Size = node["Size"].as<glm::vec3>();
			if (node["material"])
				material = node["material"].as<PhysicsMaterial>();
		}
	private:
		friend class PhysicsScene;
		glm::vec3 Offset = glm::vec3(0.0f);
		glm::vec3 Size = glm::vec3(.5f);

	};
	class STULU_API SphereColliderComponent : public Collider{
	public:
		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;

		virtual void Create(PhysicsScene* physics);

		glm::vec3 GetOffset() const { return Offset; }
		float GetRadius() const { return Radius; }

		void SetOffset(glm::vec3 value);
		void SetRadius(float value);

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "Offset" << YAML::Value << Offset;
			out << YAML::Key << "Radius" << YAML::Value << Radius;
			out << YAML::Key << "material" << YAML::Value << material;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["Offset"])
				Offset = node["Offset"].as<glm::vec3>();
			if (node["Radius"])
				Radius = node["Radius"].as<float>();
			if (node["material"])
				material = node["material"].as<PhysicsMaterial>();
		}
	private:
		friend class PhysicsScene;
		glm::vec3 Offset = glm::vec3(0.0f);
		float Radius = .5f;

	};
	class STULU_API CapsuleColliderComponent : public Collider {
	public:
		CapsuleColliderComponent() = default;
		CapsuleColliderComponent(const CapsuleColliderComponent&) = default;

		virtual void Create(PhysicsScene* physics);

		glm::vec3 GetOffset() const { return Offset; }
		float GetRadius() const { return Radius; }
		float GetHeight() const { return Height; }
		bool GetVertical() const { return Vertical; }

		void SetOffset(glm::vec3 value);
		void SetRadius(float value);
		void SetHeight(float value);
		void SetVertical(bool value);

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "Offset" << YAML::Value << Offset;
			out << YAML::Key << "Radius" << YAML::Value << Radius;
			out << YAML::Key << "Height" << YAML::Value << Height;
			out << YAML::Key << "Vertical" << YAML::Value << Vertical;
			out << YAML::Key << "material" << YAML::Value << material;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["Offset"])
				Offset = node["Offset"].as<glm::vec3>();
			if (node["Radius"])
				Radius = node["Radius"].as<float>();
			if (node["Height"])
				Height = node["Height"].as<float>();
			if (node["Vertical"])
				Vertical = node["Vertical"].as<bool>();
			if (node["material"])
				material = node["material"].as<PhysicsMaterial>();
		}
	private:
		friend class PhysicsScene;

		glm::vec3 Offset = glm::vec3(0.0f);
		float Radius = 0.5f;
		float Height = 2.0f;
		bool Vertical = true;
	};
	class STULU_API MeshColliderComponent : public Collider {
	public:
		MeshColliderComponent() = default;
		MeshColliderComponent(const MeshColliderComponent&) = default;

		virtual void Create(PhysicsScene* physics);

		void BuildConvex();

		const MeshAsset& GetMesh() const {
			return Mesh;
		}
		bool GetConvex() const {
			return Convex;
		}

		void SetMesh(const MeshAsset& asset);

		void SetConvex(bool value) {
			Convex = value;
		}

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "Convex" << YAML::Value << Convex;
			out << YAML::Key << "material" << YAML::Value << material;
			if (Mesh.IsValid())
				out << YAML::Key << "Mesh" << YAML::Value << Mesh.GetUUID();
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["Convex"])
				Convex = node["Convex"].as<bool>();
			if (node["Mesh"])
				Mesh = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(node["Mesh"].as<UUID>());
			if (node["material"])
				material = node["material"].as<PhysicsMaterial>();
		}
	private:
		friend class PhysicsScene;
		MeshAsset Mesh;
		bool Convex = false;
	};
}