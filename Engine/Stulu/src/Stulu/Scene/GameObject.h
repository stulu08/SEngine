#pragma once
#include <entt.hpp>
#include "Scene.h"

namespace Stulu {
	class GameObject {
	public:
		inline GameObject() = default;
		inline GameObject(entt::entity entity, Scene* scene)
			: m_entity(entity), m_scene(scene) {
		}
		inline GameObject(const GameObject& other) = default;

		template<typename T>
		inline bool hasComponent() const {
			return m_scene->m_registry.storage<T>().contains(m_entity);
		}
		template<typename T, typename... Args>
		inline T& addComponent(Args&&... args) {
			if (hasComponent<T>()) {
				CORE_WARN("GameObject already has component, returning component");
				return getComponent<T>();
			}
			T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
			component.gameObject = { m_entity,m_scene };
			m_scene->onComponentAdded<T>(*this, component);
			return component;
		}
		template<typename T, typename... Args>
		inline T& saveAddComponent(Args&&... args) {
			if (hasComponent<T>())
				return getComponent<T>();
			T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
			m_scene->onComponentAdded<T>(*this, component);
			component.gameObject = {m_entity,m_scene};
			return component;
		}
		template<typename T, typename... Args>
		inline T& addOrReplaceComponent(Args&&... args) {
			T& component = m_scene->m_registry.emplace_or_replace<T>(m_entity, std::forward<Args>(args)...);
			component.gameObject = { m_entity,m_scene };
			m_scene->onComponentAdded<T>(*this, component);
			return component;
		}
		template<typename T>
		inline T& getComponent() const {
			CORE_ASSERT(hasComponent<T>(), "GameObject does not have component");
			return m_scene->m_registry.get<T>(m_entity);
		}
		template<typename T>
		inline bool saveGetComponent(T& t) const {
			if (m_scene->m_registry.storage<T>().contains(m_entity)) {
				t = m_scene->m_registry.get<T>(m_entity);
				return true;
			}
			return false;
		}
		template<typename T>
		inline bool removeComponent() {
			if (!hasComponent<T>()) {
				CORE_ERROR("GameObject does not have component");
				return false;
			}
			m_scene->onComponentRemove<T>(*this, getComponent<T>());
			m_scene->m_registry.remove<T>(m_entity);
			return true;
		}
		template<typename T>
		inline bool saveRemoveComponent() {
			if (!hasComponent<T>()) {
				return false;
			}
			m_scene->onComponentRemove<T>(*this, getComponent<T>());
			m_scene->m_registry.remove<T>(m_entity);
			return true;
		}

		inline Scene* getScene() const { return m_scene; }

		STULU_API bool isValid() const;
		STULU_API entt::entity getId() const;
		STULU_API static GameObject getById(entt::entity id, Scene* scene);

		inline operator bool() const { 
			return isValid(); 
		}
		inline operator entt::entity() const { return m_entity; }
		inline operator uint64_t() const { return (uint32_t)m_entity; }
		inline operator void*() const { return (void*)(uint64_t)m_entity; }

		inline bool operator==(const GameObject& other) const {
			// if both ids are invalid they are same, important if (gameobject == GameObject::null)
			if (other.m_entity == entt::null && this->m_entity == entt::null)
				return true;

			return m_entity == other.m_entity && m_scene == other.m_scene;
		}
		inline bool operator!=(const GameObject& other) const {
			return !(*this == other);
		}

		static GameObject null;
	private:
		entt::entity m_entity{ entt::null };
		Scene* m_scene = nullptr;

		friend class EditorInspectorPanel;
		friend class Scene;
	};

	inline GameObject GameObject::null = { entt::entity{ entt::null }, nullptr };
}

