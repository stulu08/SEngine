#pragma once
#include "Types.h"

#include "Stulu/Scene/GameObject.h"
#include <Stulu/Scene/Components/Components.h>
#include <Stulu/Core/Resources.h>

namespace StuluBindings {
	class GameObject {
	public:	
		static inline void addComponent(uint32_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				auto& componentRegister = getManager()->GetComponentRegister_Add();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				auto assemblyManager = getManager();
				MonoClass* clazz = mono_type_get_class(type);
				if (!clazz)
					return;
				const auto& classes = assemblyManager->getAssembly()->getClasses();

				Stulu::MonoClassEntry classEntry;
				classEntry.name = mono_class_get_name(clazz);
				classEntry.nameSpace = mono_class_get_namespace(clazz);

				if (std::find(classes.begin(), classes.end(), classEntry) != classes.end()) {
					Stulu::ScriptingComponent& comp = gameObject.saveAddComponent<Stulu::ScriptingComponent>();
					auto object = Stulu::createRef<Stulu::MonoObjectInstance>(classEntry.nameSpace, classEntry.name, assemblyManager->getAssembly().get());
					object->loadAll();
					gameObject.getScene()->initScriptRuntime(object, gameObject);
					comp.runtimeScripts.push_back(object);
				}
			}
		}
		static inline bool hasComponent(uint32_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				auto& componentRegister = getManager()->GetComponentRegister_Has();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				auto assemblyManager = getManager();
				MonoClass* clazz = mono_type_get_class(type);
				if (!clazz)
					return false;
				auto& classes = assemblyManager->getAssembly()->getClasses();

				Stulu::MonoClassEntry classEntry;
				classEntry.name = mono_class_get_name(clazz);
				classEntry.nameSpace = mono_class_get_namespace(clazz);

				if (std::find(classes.begin(), classes.end(), classEntry) != classes.end()) {
					auto& comp = gameObject.saveAddComponent<Stulu::ScriptingComponent>();
					for (auto& script : comp.runtimeScripts) {
						if (script->getClassPtr() == clazz) {
							return true;
						}
					}
				}
			}
			return false;
		}
		static inline bool removeComponent(uint32_t go, MonoReflectionType* reftype) {
			MonoType* type = mono_reflection_type_get_type(reftype);
			if (type) {
				auto& componentRegister = getManager()->GetComponentRegister_Remove();
				Stulu::GameObject gameObject = Stulu::GameObject((entt::entity)go, Stulu::Scene::activeScene());
				std::string typeName = mono_type_get_name_full(type, MonoTypeNameFormat::MONO_TYPE_NAME_FORMAT_FULL_NAME);
				if (componentRegister.find(typeName) != componentRegister.end()) {
					return componentRegister[typeName](gameObject);
				}

				// c# scripts
				auto assemblyManager = getManager();
				MonoClass* clazz = mono_type_get_class(type);
				if (!clazz)
					return false;
				const auto& classes = assemblyManager->getAssembly()->getClasses();

				Stulu::MonoClassEntry classEntry;
				classEntry.name = mono_class_get_name(clazz);
				classEntry.nameSpace = mono_class_get_namespace(clazz);

				if (std::find(classes.begin(), classes.end(), classEntry) != classes.end()) {
					auto& comp = gameObject.saveAddComponent<Stulu::ScriptingComponent>();
					for (auto script : comp.runtimeScripts) {
						if (script->getClassPtr() == clazz) {
							comp.runtimeScripts.erase(std::find(comp.runtimeScripts.begin(), comp.runtimeScripts.end(), script));
							return true;
						}
					}
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
		static inline uint32_t createSphere(MonoString* monoName, MonoString* monoTag, Vector3 position) {
			std::string name = mono_string_to_utf8(monoName);
			std::string tag = mono_string_to_utf8(monoTag);
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(name);

			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = tag;
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			base.gameObject = go;
			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getSphereMeshAsset();
			go.addComponent<Stulu::SphereColliderComponent>().create(go, Stulu::Scene::activeScene()->getPhysics());
			return go;
		}
		static inline uint32_t createCube(MonoString* monoName, MonoString* monoTag, Vector3 position) {
			std::string name = mono_string_to_utf8(monoName);
			std::string tag = mono_string_to_utf8(monoTag);
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(name);

			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = tag;
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			base.gameObject = go;
			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getCubeMeshAsset();
			go.addComponent<Stulu::BoxColliderComponent>().create(go, Stulu::Scene::activeScene()->getPhysics());
			return go;
		}
		static inline uint32_t createCapsule(MonoString* monoName, MonoString* monoTag, Vector3 position) {
			std::string name = mono_string_to_utf8(monoName);
			std::string tag = mono_string_to_utf8(monoTag);
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(name);

			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = tag;
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			base.gameObject = go;
			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getCapsuleMeshAsset();
			go.addComponent<Stulu::CapsuleColliderComponent>().create(go, Stulu::Scene::activeScene()->getPhysics());
			return go;
		}
		static inline uint32_t createPlane(MonoString* monoName, MonoString* monoTag, Vector3 position) {
			std::string name = mono_string_to_utf8(monoName);
			std::string tag = mono_string_to_utf8(monoTag);
			Stulu::GameObject go = Stulu::Scene::activeScene()->createGameObject(name);

			auto& base = go.getComponent<Stulu::GameObjectBaseComponent>();
			base.tag = tag;
			auto& tc = go.getComponent<Stulu::TransformComponent>();
			tc.position = position.toNative_s();
			base.gameObject = go;
			go.addComponent<Stulu::MeshFilterComponent>().mesh = Stulu::Resources::getPlaneMeshAsset();
			go.addComponent<Stulu::MeshColliderComponent>().mesh = Stulu::Resources::getPlaneMeshAsset();
			return go;
		}
		static inline void destroy(uint32_t id) {
			Stulu::Scene::activeScene()->destroyGameObject(Stulu::GameObject((entt::entity)id, Stulu::Scene::activeScene()));
		}
	};
}