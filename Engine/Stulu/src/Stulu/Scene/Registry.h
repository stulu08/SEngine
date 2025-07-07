#pragma once
#include "Stulu/Core/Core.h"
#include "Stulu/Core/Log.h"
#include "Stulu/Types/UUID.h"
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
		
		inline bool IsValid(entt::entity go) const {
			return go != entt::null && m_registry.valid(go);
		}

		inline entt::registry& GetRegistry() { return m_registry; }
		GameObject FindByName(const std::string& name);

		virtual bool IsScene() const { return false; }
		virtual Scene* GetAsScene() { return nullptr; }
	protected:
		entt::registry m_registry;

		template<typename T>
		inline void onComponentAdded(GameObject gameObject, T& component) {
			component.gameObject = gameObject;
			component.onComponentAdded(this);
		}
		template<typename T>
		inline void onComponentRemove(GameObject gameObject, T& component) {
			component.gameObject = gameObject;
			component.onComponentRemove(this);
		}

		// only for internal usage
		GameObject CreateEmpty(entt::entity id = entt::null);
		
		friend class GameObject;
	};
}