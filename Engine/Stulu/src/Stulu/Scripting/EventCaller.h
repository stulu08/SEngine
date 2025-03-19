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

		void NativeSceneStart();
		void NativeGameObjectCreate(const GameObject& object);
		void NativeGameObjectDestory(const GameObject& object);

		void ConstructManaged(const GameObject& object = GameObject::null);
		void InitManagedRuntimeScript(const GameObject& gameObject, Ref<MonoObjectInstance>& script);
		bool InitManagedGameObject(const GameObject& gameObject, Ref<MonoObjectInstance>& script);

		void SerializerGameObject(YAML::Emitter& out, GameObject& gameObject);
		void DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path);

		void CallManagedEvent(Mono::Method method, const GameObject& object = GameObject::null);

		template<class T>
		inline bool HasLayer() const {
			return m_layer.find(typeid(T).hash_code()) != m_layer.end();
		}
		template<class T>
		inline T& GetLayer() {
			return *dynamic_cast<T*>(m_layer.at(typeid(T).hash_code()));
		}

		template<class T>
		static inline void RegisterLayer() {
			s_registeredSceneLayers[typeid(m_layer).hash_code()] = CreateSceneLayer<T>;
		}
	private:
		Scene* m_scene;
		std::unordered_map<size_t, SceneLayer*> m_layer;
		Ref<AssemblyManager> m_manager;

		static std::unordered_map<size_t, std::function<std::pair<size_t, SceneLayer*>(Scene*)>> s_registeredSceneLayers;

		template<class T>
		static inline std::pair<size_t, SceneLayer*> CreateSceneLayer(Scene* scene) {
			T* layer = new T();
			layer->Initlize(scene);
			return { typeid(T).hash_code(), layer };
		}
	};
}