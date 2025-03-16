#pragma once
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Core/Application.h"
namespace Stulu {
	class STULU_API Component {
	public:
		inline Component() {

		}
		inline Component(const Component& other) {
			this->gameObject = other.gameObject;
		}

		virtual ~Component() = default;

		//this GameObject
		GameObject gameObject = GameObject::null;

		virtual void onComponentAdded(Scene* scene) {};
		virtual void onComponentRemove(Scene* scene) {};

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
		static std::unordered_map<size_t, std::function<void(entt::registry&, entt::registry&, Scene*)>> m_componentCopyList;

		template<class T>
		static inline void CopyComponent(entt::registry& dst, entt::registry& src, Scene* scene) {
			auto view = src.view<T>();
			for (auto srcGameObject : view)
			{
				// they have the same entt id
				entt::entity dstGameObject = srcGameObject;
				T& srcComponent = src.get<T>(srcGameObject);
				
				// so this->gameObject can be used inside the copy constructor
				GameObject oldGameObject = srcComponent.gameObject;
				GameObject newGameObject = GameObject(dstGameObject, scene);
				srcComponent.gameObject = newGameObject;

				dst.emplace_or_replace<T>(dstGameObject, srcComponent).gameObject = newGameObject;

				// set the GameObject back
				srcComponent.gameObject = oldGameObject;
			}
		}

		friend class Scene;
	};

	class GameObjectBaseComponent : public Component {
	public:
		std::string name = "GameObject";
		std::string tag = "Default";

		GameObjectBaseComponent() = default;
		GameObjectBaseComponent(const GameObjectBaseComponent&) = default;
		GameObjectBaseComponent(const std::string& name)
			: name(name) {};
	};
	// defined here to keep the function inline and prevent Declaraton problems
	inline bool GameObject::IsValid() const {
		if (m_entity == entt::null || m_scene == nullptr)
			return false;
		return hasComponent<GameObjectBaseComponent>();
	}
}