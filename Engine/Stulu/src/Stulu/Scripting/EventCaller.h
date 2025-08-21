#pragma once
#include "Stulu/Types/Stack.h"
#include "Stulu/Scene/GameObject.h"
#include "Managed/AssemblyManager.h"
#include "Native/SceneLayer.h"

namespace Stulu {
	class STULU_API EventCaller {
	public:
		EventCaller(Scene* scene);
		EventCaller(Scene* scene, Scene* copyTarget);

		~EventCaller();

		void onAwake(const GameObject& object = GameObject::null);
		void onStart(const GameObject& object = GameObject::null);
		void onUpdate(const GameObject& object = GameObject::null);
		void onRender(const GameObject& object = GameObject::null);
		void onRender2D(const GameObject& object = GameObject::null);
		void onDrawGizmos(const GameObject& object = GameObject::null);
		void onDestroy(const GameObject& object = GameObject::null);
		void onSceneExit(const GameObject& object = GameObject::null);

		void NativeSceneStart();
		void NativePreUpdate();
		void NativeGameObjectCreate(const GameObject& object);
		void NativeGameObjectDestory(const GameObject& object);

		void ConstructManaged(const GameObject& object = GameObject::null);
		void InitManagedRuntimeScript(const GameObject& gameObject, Ref<MonoObjectInstance>& script);
		bool InitManagedGameObject(const GameObject& gameObject, Ref<MonoObjectInstance>& script);

		void SerializerScene(YAML::Emitter& out);
		void DeserializerScene(YAML::Node& data);

		void CallManagedEvent(Mono::Method method, const GameObject& object = GameObject::null);

		inline bool HasLayer(size_t id) const {
			return m_layer.find(id) != m_layer.end();
		}
		template<class T>
		inline bool HasLayer() const {
			return HasLayer(typeid(T).hash_code());
		}

		inline SceneLayer* GetLayer(size_t id) {
			return m_layer.at(id);
		}
		template<class T>
		inline T& GetLayer() {
			return *dynamic_cast<T*>(GetLayer(typeid(T).hash_code()));
		}

		template<class T>
		static inline void RegisterLayer() {
			s_registeredSceneLayers[typeid(T).hash_code()] = { CreateSceneLayer<T>, CopySceneLayer<T> };
		}
		static inline void ClearRegisteredLayers() {
			s_registeredSceneLayers.clear();
		}
	private:
		Scene* m_scene;
		std::unordered_map<size_t, SceneLayer*> m_layer;
		Ref<AssemblyManager> m_manager;
		Mono::Method m_initManagedMethod;

		using LayerCreateFn = std::function<std::pair<size_t, SceneLayer*>(Scene*)>;
		using LayerCopyFn = std::function<std::pair<size_t, SceneLayer*>(Scene*, const SceneLayer*)>;
		using LayerRegistryEntry = std::pair<LayerCreateFn, LayerCopyFn>;

		static std::unordered_map<size_t, LayerRegistryEntry> s_registeredSceneLayers;

		template<class T>
		static inline std::pair<size_t, SceneLayer*> CreateSceneLayer(Scene* scene) {
			T* layer = new T();
			if (layer->Initlize(scene))
				return { typeid(T).hash_code(), layer };
			else
				return { 0, nullptr };
		}
		template<class T>
		static inline std::pair<size_t, SceneLayer*> CopySceneLayer(Scene* scene, const SceneLayer* copy) {
			T* layer = new T(*static_cast<const T*>(copy));
			if (layer->Initlize(scene))
				return { typeid(T).hash_code(), layer };
			else
				return { 0, nullptr };
		}
	};
}