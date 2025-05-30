#pragma once
#include <Stulu/Physics/Components/RigidbodyComponent.h>
#include <Stulu/Physics/Components/Collider.h>

namespace StuluBindings {
	class GameObject {
	public:	
		static inline void addComponent(uint64_t go, Stulu::Mono::ReflectionType reftype) {
			Stulu::Mono::Type type = reftype.GetType();
			if (type) {
				const auto& manager = getManager();

				auto& componentRegister = manager->GetComponentRegister_Add();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
				std::string typeName = type.GetNameFull(Stulu::Mono::TypeNameFormat::FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				Stulu::Mono::Class desiredClass = type.GetClass();
				if (!desiredClass)
					return;

				Stulu::Mono::Class parent = desiredClass.GetParent();
				if (!parent)
					return;

				if (parent == manager->getComponentClass()) {
					getManager()->ManagedAddComponent(gameObject, desiredClass);
				}
			}
		}
		static inline bool hasComponent(uint64_t go, Stulu::Mono::ReflectionType reftype) {
			Stulu::Mono::Type type = reftype.GetType();
			if (type) {
				const auto& manager = getManager();

				auto& componentRegister = manager->GetComponentRegister_Has();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
				std::string typeName = type.GetNameFull(Stulu::Mono::TypeNameFormat::FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				Stulu::Mono::Class desiredClass = type.GetClass();
				if (!desiredClass)
					return false;

				Stulu::Mono::Class parent = desiredClass.GetParent();
				if (!parent)
					return false;

				if (parent == manager->getComponentClass()) {
					return getManager()->ManagedHasComponent(gameObject, desiredClass);
				}
			}
			return false;
		}
		static inline bool removeComponent(uint64_t go, Stulu::Mono::ReflectionType reftype) {
			Stulu::Mono::Type type = reftype.GetType();
			if (type) {
				const auto& manager = getManager();

				auto& componentRegister = manager->GetComponentRegister_Remove();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
				std::string typeName = type.GetNameFull(Stulu::Mono::TypeNameFormat::FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				Stulu::Mono::Class desiredClass = type.GetClass();
				if (!desiredClass)
					return false;

				Stulu::Mono::Class parent = desiredClass.GetParent();
				if (!parent)
					return false;

				if (parent == manager->getComponentClass()) {
					return getManager()->ManagedRemoveComponent(gameObject, desiredClass);
				}
			}
			return false;
		}
		// only c# scripts
		static inline MonoObject* getComponent(uint64_t go, Stulu::Mono::ReflectionType reftype) {
			Stulu::Mono::Type type = reftype.GetType();
			if (type) {
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
				if (gameObject != Stulu::GameObject::null) {
					Stulu::Mono::Class desired = type.GetClass();
					Stulu::Mono::Object object = getManager()->ManagedGetComponent(gameObject, desired);
					if (object)
						return (MonoObject*)object;
				}
				
			}
			CORE_ERROR("Invalid type");
			return nullptr;
		}
		static inline uint64_t create(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position, Quaternion rotation, Vector3 scale) {
			Stulu::GameObject go = GetCurrentRegistry()->Create(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			tc.rotation = rotation.toNative();
			tc.scale = scale.toNative_s();
			return (uint64_t)go.GetID();
		}
		static inline void setName(uint64_t go, Stulu::Mono::String name) {
			Stulu::GameObject((entt::entity)go, GetCurrentRegistry()).getComponent<Stulu::GameObjectBaseComponent>().name = name.ToUtf8();
		}
		static inline Stulu::Mono::String getName(uint64_t go) {
			return Stulu::Mono::String::New(getCoreDomain(), Stulu::GameObject((entt::entity)go, GetCurrentRegistry()).getComponent<Stulu::GameObjectBaseComponent>().name);
		}
		static inline void setTag(uint64_t go, Stulu::Mono::String tag) {
			Stulu::GameObject((entt::entity)go, GetCurrentRegistry()).getComponent<Stulu::GameObjectBaseComponent>().tag = tag.ToUtf8();
		}
		static inline Stulu::Mono::String getTag(uint64_t go) {
			return Stulu::Mono::String::New(getCoreDomain(), Stulu::GameObject((entt::entity)go, GetCurrentRegistry()).getComponent<Stulu::GameObjectBaseComponent>().tag);
		}
		static inline uint64_t createSphere(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = GetCurrentRegistry()->Create(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();

			go.addComponent<Stulu::MeshFilterComponent>().SetMesh(Stulu::Resources::SphereMesh());
			go.addComponent<Stulu::MeshRendererComponent>();
			go.addComponent<Stulu::RigidbodyComponent>();
			go.addComponent<Stulu::SphereColliderComponent>();
			return (uint64_t)go.GetID();
		}
		static inline uint64_t createCube(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = GetCurrentRegistry()->Create(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();

			go.addComponent<Stulu::MeshFilterComponent>().SetMesh(Stulu::Resources::CubeMesh());
			go.addComponent<Stulu::MeshRendererComponent>();
			go.addComponent<Stulu::RigidbodyComponent>();
			go.addComponent<Stulu::BoxColliderComponent>();
			return (uint64_t)go.GetID();
		}
		static inline uint64_t createCapsule(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = GetCurrentRegistry()->Create(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			go.addComponent<Stulu::MeshFilterComponent>().SetMesh(Stulu::Resources::CapsuleMesh());
			go.addComponent<Stulu::MeshRendererComponent>();
			go.addComponent<Stulu::RigidbodyComponent>();
			go.addComponent<Stulu::CapsuleColliderComponent>();
			return (uint64_t)go.GetID();
		}
		static inline uint64_t createPlane(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = GetCurrentRegistry()->Create(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();

			go.addComponent<Stulu::MeshFilterComponent>().SetMesh(Stulu::Resources::PlaneMesh());
			go.addComponent<Stulu::MeshRendererComponent>();
			go.addComponent<Stulu::RigidbodyComponent>();
			go.addComponent<Stulu::MeshColliderComponent>().SetMesh(Stulu::Resources::PlaneMesh());
			return (uint64_t)go.GetID();
		}
		static inline void destroy(uint64_t id) {
			GetCurrentRegistry()->Destroy(Stulu::GameObject((entt::entity)id, GetCurrentRegistry()));
		}
	};
}