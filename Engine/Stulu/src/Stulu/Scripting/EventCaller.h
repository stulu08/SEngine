#pragma once
#include "Stulu/Core/Stack.h"
#include "Stulu/Scene/GameObject.h"
#include "Managed/AssemblyManager.h"
#include "Native/SceneLayer.h"

namespace Stulu {
	class STULU_API EventCaller {
	public:
		EventCaller(Scene* scene);

		void onAwake(const GameObject& object = GameObject::null);
		void onStart(const GameObject& object = GameObject::null);
		void onUpdate(const GameObject& object = GameObject::null);
		void onRender(const GameObject& object = GameObject::null);
		void onRender2D(const GameObject& object = GameObject::null);
		void onDrawGizmos(const GameObject& object = GameObject::null);
		void onDestroy(const GameObject& object = GameObject::null);
		void onSceneExit(const GameObject& object = GameObject::null);

		void GameObjectCreate(const GameObject& object);
		void GameObjectDestory(const GameObject& object);

		void ConstructManaged(const GameObject& object = GameObject::null);
		void InitManagedRuntimeScript(const GameObject& gameObject, Ref<MonoObjectInstance>& script);
		bool InitManagedGameObject(const GameObject& gameObject, Ref<MonoObjectInstance>& script);

		void SerializerGameObject(YAML::Emitter& out, GameObject& gameObject);
		void DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path);

		void CallManagedEvent(Mono::Method method, const GameObject& object = GameObject::null);

		template<class T>
		static inline void RegisterLayer() {
			s_registeredSceneLayers[typeid(T).hash_code()] = CreateSceneLayer<T>;
		}
	private:
		Scene* m_scene;
		Stack<SceneLayer> m_layer;
		Ref<AssemblyManager> m_manager;

		static std::unordered_map<size_t, std::function<SceneLayer* (Scene*)>> s_registeredSceneLayers;

		template<class T>
		static inline SceneLayer* CreateSceneLayer(Scene* scene) {
			T* layer = new T();
			layer->Initlize(scene);
			return layer;
		}
	};
}