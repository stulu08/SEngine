#pragma once
#include <entt.hpp>
#include "Registry.h"

namespace Stulu {
	class GameObject {
	public:
		inline GameObject() = default;
		inline GameObject(entt::entity entity, Registry* registry)
			: m_entity(entity), m_registry(registry) {
		}
		inline GameObject(const GameObject& other) = default;

		STULU_API static GameObject GetById(entt::entity id, Registry* registry);

		template<typename T>
		inline bool hasComponent() const {
			return m_registry->HasComponent<T>(m_entity);
		}
		template<typename T, typename... Args>
		inline T& addComponent(Args&&... args) const {
			if (hasComponent<T>()) {
				CORE_WARN("GameObject already has component, returning component");
				return m_registry->GetComponent<T>(m_entity);
			}
			T& component = m_registry->AddComponent<T>(m_entity, std::forward<Args>(args)...);
			return component;
		}
		template<typename T, typename... Args>
		inline T& saveAddComponent(Args&&... args) const {
			if (hasComponent<T>()) 
				return m_registry->GetComponent<T>(m_entity);

			T& component = m_registry->AddComponent<T>(m_entity, std::forward<Args>(args)...);
			return component;
		}
		template<typename T>
		inline T& getComponent() const {
			CORE_ASSERT(hasComponent<T>(), "GameObject does not have component");
			return m_registry->GetComponent<T>(m_entity);
		}
		template<class T>
		inline T* tryGetComponent() const {
			return m_registry->TryGetComponent<T>(m_entity);
		}
		template<typename T>
		inline bool removeComponent() const {
			return m_registry->RemoveComponent<T>(m_entity);;
		}

		inline Registry* GetRegistry() { return m_registry; }
		inline Registry* GetRegistry() const { return m_registry; }

		// defined after GameObjectBaseComponent to keep the function inline
		inline bool IsValid() const;

		inline entt::entity GetID() const {
			return m_entity;
		}

		inline operator bool() const { 
			return IsValid(); 
		}

		inline bool operator==(const GameObject& other) const {
			// if both ids are invalid they are same, important if (gameobject == GameObject::null)
			if (other.GetID() == entt::null && this->GetID() == entt::null)
				return true;

			return GetID() == other.GetID() && m_registry == other.m_registry;
		}
		inline bool operator!=(const GameObject& other) const {
			return !(*this == other);
		}

		static GameObject null;
	private:
		entt::entity m_entity{ entt::null };
		Registry* m_registry = nullptr;
	};

	inline GameObject GameObject::null = { entt::entity{ entt::null }, nullptr };
}

