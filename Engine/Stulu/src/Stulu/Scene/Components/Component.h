#pragma once
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Core/Application.h"
namespace Stulu {
	class STULU_API Component {
	public:
		//this GameObject
		GameObject gameObject = GameObject::null;

		virtual void destroy() {};
		virtual void onComponentAdded(Scene* scene) {};

		template<class T>
		static inline void RegisterCopy() {
			m_componentCopyList[typeid(T).hash_code()] = CopyComponent<T>;
		}
		template<class T>
		static inline void RegisterManaged(const std::string& managedName) {
			Application::get().getAssemblyManager()->RegisterComponent<T>(managedName);
		}
		template<class T>
		static inline void Register(const std::string& managedName) {
			RegisterCopy<T>();
			RegisterManaged<T>(managedName);
		}

		static void RegisterBaseComponents();
	private:
		static std::unordered_map<size_t, std::function<void(entt::registry&, entt::registry&)>> m_componentCopyList;

		template<class T>
		static inline void CopyComponent(entt::registry& dst, entt::registry& src) {
			auto view = src.view<T>();
			for (auto srcGameObject : view)
			{
				// they have the same entt id
				entt::entity dstGameObject = srcGameObject;

				T& srcComponent = src.get<T>(srcGameObject);
				dst.emplace_or_replace<T>(dstGameObject, srcComponent);
				dst.get<T>(dstGameObject).gameObject = GameObject(dstGameObject, Scene::activeScene());
			}
		}

		friend class Scene;
	};
}