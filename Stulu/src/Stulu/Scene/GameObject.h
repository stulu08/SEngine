#pragma once
#include <entt.hpp>
#include "Scene.h"

namespace Stulu {
	class STULU_API GameObject {
	public:
		GameObject() = default;
		GameObject(entt::entity entity, Scene* scene);
		GameObject(const GameObject& other) = default;

		template<typename T>
		bool hasComponent() {
			return m_scene->m_registry.has<T>(m_entity);
		}
		template<typename T, typename... Args>
		T& addComponent(Args&&... args) {
			if (hasComponent<T>()) {
				CORE_WARN("GameObject already has component, returning component");
				return getComponent<T>();
			}
			T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
			m_scene->onComponentAdded<T>(*this, component);
			return component;
		}
		template<typename T, typename... Args>
		T& saveAddComponent(Args&&... args) {
			if (hasComponent<T>())
				return getComponent<T>();
			T& component = m_scene->m_registry.emplace<T>(m_entity, std::forward<Args>(args)...);
			m_scene->onComponentAdded<T>(*this, component);
			return component;
		}
		template<typename T>
		T& getComponent() {
			CORE_ASSERT(hasComponent<T>(), "GameObject does not have component");
			return m_scene->m_registry.get<T>(m_entity);
		}
		template<typename T>
		bool saveGetComponent(T& t) {
			if (m_scene->m_registry.has<T>(m_entity)) {
				t = m_scene->m_registry.get<T>(m_entity);
				return true;
			}
			return false;
		}
		template<typename T>
		void removeComponent() {
			if (!hasComponent<T>()) {
				CORE_ERROR("GameObject does not have component");
				return;
			}
			m_scene->onComponentRemove<T>(*this, getComponent<T>());
			m_scene->m_registry.remove<T>(m_entity);
		}

		UUID getId();
		static GameObject getById(UUID& id, Scene* scene);

		operator bool() const { 
			return m_scene != nullptr || m_entity != entt::null; 
		}
		operator entt::entity() const { return m_entity; }
		operator uint32_t() const { return (uint32_t)m_entity; }
		operator void*() const { return (void*)(uint64_t)m_entity; }

		bool operator==(const GameObject& other) const {
			return m_entity == other.m_entity && m_scene == other.m_scene;
		}
		bool operator!=(const GameObject& other) const {
			return !(*this == other);
		}

		static GameObject null;
	private:
		entt::entity m_entity{ entt::null };
		Scene* m_scene = nullptr;

		friend class EditorInspectorPanel;
		friend class Scene;
	};
}

