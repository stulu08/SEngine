#pragma once

#include <mono/metadata/reflection.h>

namespace StuluBindings {
	class GameObject {
	public:	
		static inline void addComponent(uint64_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				const auto& manager = getManager();

				auto& componentRegister = manager->GetComponentRegister_Add();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				Stulu::Mono::Class desiredClass = mono_type_get_class(type);
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
		static inline bool hasComponent(uint64_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				const auto& manager = getManager();

				auto& componentRegister = manager->GetComponentRegister_Has();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				Stulu::Mono::Class desiredClass = mono_type_get_class(type);
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
		static inline bool removeComponent(uint64_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				const auto& manager = getManager();

				auto& componentRegister = manager->GetComponentRegister_Remove();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				Stulu::Mono::Class desiredClass = mono_type_get_class(type);
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
		static inline MonoObject* getComponent(uint64_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				if (gameObject != Stulu::GameObject::null) {
					Stulu::Mono::Class desired = mono_type_get_class(type);
					Stulu::Mono::Object object = getManager()->ManagedGetComponent(gameObject, desired);
					if (object)
						return (MonoObject*)object;
				}
				
			}
			CORE_ERROR("Invalid type");
			return nullptr;
		}
		static inline uint64_t create(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position, Quaternion rotation, Vector3 scale) {
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			tc.rotation = rotation.toNative();
			tc.scale = scale.toNative_s();
			return go;
		}
		static inline void setName(uint64_t go, Stulu::Mono::String name) {
			Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().name = name.ToUtf8();
		}
		static inline Stulu::Mono::String getName(uint64_t go) {
			return Stulu::Mono::String::New(getCoreDomain(), Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().name);
		}
		static inline void setTag(uint64_t go, Stulu::Mono::String tag) {
			Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().tag = tag.ToUtf8();
		}
		static inline Stulu::Mono::String getTag(uint64_t go) {
			return Stulu::Mono::String::New(getCoreDomain(), Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().tag);
		}
		static inline uint64_t createSphere(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();

			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getSphereMeshAsset();
			go.addComponent<Stulu::SphereColliderComponent>().create(go, Stulu::Scene::activeScene()->getPhysics());
			return go;
		}
		static inline uint64_t createCube(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();

			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getCubeMeshAsset();
			go.addComponent<Stulu::BoxColliderComponent>().create(go, Stulu::Scene::activeScene()->getPhysics());
			return go;
		}
		static inline uint64_t createCapsule(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getCapsuleMeshAsset();
			go.addComponent<Stulu::CapsuleColliderComponent>().create(go, Stulu::Scene::activeScene()->getPhysics());
			return go;
		}
		static inline uint64_t createPlane(Stulu::Mono::String monoName, Stulu::Mono::String monoTag, Vector3 position) {
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(monoName.ToUtf8());
			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = monoTag.ToUtf8();
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();

			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getPlaneMeshAsset();
			go.addComponent<Stulu::MeshColliderComponent>().mesh = Stulu::Resources::getPlaneMeshAsset();
			return go;
		}
		static inline void destroy(uint64_t id) {
			Stulu::Scene::activeScene()->destroyGameObject(Stulu::GameObject((entt::entity)id, Stulu::Scene::activeScene()));
		}
	};
}