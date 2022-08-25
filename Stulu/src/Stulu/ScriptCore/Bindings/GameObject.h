#pragma once
#include "Stulu/Scene/GameObject.h"
#include <Stulu/Scene/Components/Components.h>
#define StuluGameObject_RegisterComponent(Name, Type) \
{\
	using namespace Stulu; \
	::StuluBindings::GameObject::addComponentRegister[Name] = [](::Stulu::GameObject go) { go.addComponent<Type>(); }; \
	::StuluBindings::GameObject::hasComponentRegister[Name] = [](::Stulu::GameObject go) -> bool { return go.hasComponent<Type>(); }; \
	::StuluBindings::GameObject::removeComponentRegister[Name] = [](::Stulu::GameObject go) -> bool { return go.removeComponent<Type>(); }; \
}

namespace StuluBindings {
	class GameObject {
	public:
		static inline std::unordered_map<std::string, std::function<void(::Stulu::GameObject)>> addComponentRegister;
		static inline std::unordered_map<std::string, std::function<bool(::Stulu::GameObject)>> hasComponentRegister;
		static inline std::unordered_map<std::string, std::function<bool(::Stulu::GameObject)>> removeComponentRegister;
	
		static inline void addComponent(uint32_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (addComponentRegister.find(typeName) != addComponentRegister.end()) {
					return addComponentRegister[typeName](Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()));
				}
			}
		}
		static inline bool hasComponent(uint32_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (hasComponentRegister.find(typeName) != hasComponentRegister.end()) {
					return hasComponentRegister[typeName](Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()));
				}
			}
			return false;
		}
		static inline bool removeComponent(uint32_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (removeComponentRegister.find(typeName) != removeComponentRegister.end()) {
					return removeComponentRegister[typeName](Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()));
				}
			}
			return false;
		}
		static inline uint32_t create(MonoString* monoName, MonoString* monoTag, Vector3 position, Quaternion rotation, Vector3 scale) {
			std::string name = mono_string_to_utf8(monoName);
			std::string tag = mono_string_to_utf8(monoTag);
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(name);

			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = tag;
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			tc.rotation = rotation.toNative();
			tc.scale = scale.toNative_s();

			base.gameObject = go;

			return go;
		}
		static inline void setName(uint32_t go, MonoString* name) {
			Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().name = mono_string_to_utf8(name);
		}
		static inline MonoString* getName(uint32_t go) {
			return mono_string_new(getCoreDomain(), Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().name.c_str());
		}
		static inline void setTag(uint32_t go, MonoString* tag) {
			Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().tag = mono_string_to_utf8(tag);
		}
		static inline MonoString* getTag(uint32_t go) {
			return mono_string_new(getCoreDomain(), Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene()).getComponent<Stulu::GameObjectBaseComponent>().tag.c_str());
		}
	};
}