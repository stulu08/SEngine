#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Core/Log.h"
#include "Stulu/Types/UUID.h"
#include "Stulu/Resources/Assets/ModelAsset.h"
#include <entt.hpp>

namespace Stulu {
	class GameObject;
	class Scene;

	class STULU_API Registry {
	public:
		Registry() = default;

		virtual GameObject Create(entt::entity id = entt::null);
		virtual GameObject Create(const std::string& name, entt::entity id = entt::null);
		virtual void Destroy(GameObject gameObject);

		template<typename... Components>
		inline auto GetAllWith() {
			return m_registry.view<Components...>();
		}
		template<typename... Components>
		inline auto GetAllAsGroupWith() {
			return m_registry.group<Components...>();
		}
		template<class T>
		inline auto Each() {
			return m_registry.storage<T>().each();
		}

		GameObject FindByName(const std::string& name);

		virtual bool IsScene() const { return false; }
		virtual Scene* GetAsScene() { return nullptr; }

		virtual void SpawnModelAsset(const ModelAsset& model);
	protected:
		entt::registry m_registry;

		inline entt::registry& GetRegistry() { return m_registry; }

		template<typename T>
		inline void onComponentAdded(entt::entity goID, T& component) {
			component.gameObject = GameObject(goID, this);
			component.onComponentAdded(this);
		}
		template<typename T>
		inline void onComponentRemove(entt::entity goID, T& component) {
			component.gameObject = GameObject(goID, this);
			component.onComponentRemove(this);
		}

		// only for internal usage
		GameObject CreateEmpty(entt::entity id = entt::null);
		
		friend class GameObject;
	public:
		inline bool IsObjectValid(entt::entity id) {
			return m_registry.valid(id);
		}
		template<typename T>
		inline bool HasComponent(entt::entity id) {
			return m_registry.storage<T>().contains(id);
		}
		template<typename T, typename... Args>
		inline T& AddComponent(entt::entity id, Args&&... args) {
			T& comp = m_registry.emplace<T>(id, std::forward<Args>(args)...);
			onComponentAdded<T>(id, comp);
			return comp;
		}
		template<typename T>
		inline T& GetComponent(entt::entity id) {
			return m_registry.get<T>(id);
		}
		template<class T>
		inline T* TryGetComponent(entt::entity id) {
			return m_registry.try_get<T>(id);
		}
		template<typename T>
		inline bool RemoveComponent(entt::entity id) {
			if (T* comp = TryGetComponent<T>(id)) {
				onComponentRemove<T>(id, *comp);
				m_registry.remove<T>(id);
				return true;
			}
			return false;
		}
	};
}