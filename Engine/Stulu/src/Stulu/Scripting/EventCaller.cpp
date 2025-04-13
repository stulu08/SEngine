#include "st_pch.h"
#include "EventCaller.h"

#include "Stulu/Core/Application.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {

	std::unordered_map<size_t, EventCaller::LayerRegistryEntry> EventCaller::s_registeredSceneLayers;


	EventCaller::EventCaller(Scene* scene)
		: m_scene(scene), m_initManagedMethod(nullptr) {

		for (auto& [id, funcEntry] : s_registeredSceneLayers) {
			auto [layerHash, layerPtr] = funcEntry.first(scene);
			if (layerHash && layerPtr)
				m_layer.insert({ layerHash, layerPtr });
		}
		m_manager = Application::get().getAssemblyManager();
		m_initManagedMethod = m_manager->getGoAttachedClass().GetMethodFromName("initilize", 1);
	}

	EventCaller::EventCaller(Scene* scene, Scene* copyTarget)
		: m_scene(scene), m_initManagedMethod(nullptr) {
		
		Ref<EventCaller> targetCaller = copyTarget->getCaller();

		for (auto& [id, funcEntry] : s_registeredSceneLayers) {
			if (targetCaller->HasLayer(id)) {
				// copy layer
				auto [layerHash, layerPtr] = funcEntry.second(scene, targetCaller->GetLayer(id));
				if (layerHash && layerPtr)
					m_layer.insert({ layerHash, layerPtr });
			}
			else {
				// create layer
				auto [layerHash, layerPtr] = funcEntry.first(scene);
				if (layerHash && layerPtr)
					m_layer.insert({ layerHash, layerPtr });
			}
		}

		m_manager = Application::get().getAssemblyManager();
		m_initManagedMethod = m_manager->getGoAttachedClass().GetMethodFromName("initilize", 1);
	}

	void EventCaller::ConstructManaged(const GameObject& object) {
		if (object) {
			ScriptingComponent& comp = object.getComponent<ScriptingComponent>();
			for (auto& script : comp.runtimeScripts) {
				InitManagedRuntimeScript(object, script);
			}
			return;
		}

		for (auto& [id, comp] : m_scene->getRegistry().storage<ScriptingComponent>().each()) {
			const GameObject gameObject = { id, m_scene };
			ConstructManaged(gameObject);
		}
	}

	void EventCaller::InitManagedRuntimeScript(const GameObject& gameObject, Ref<MonoObjectInstance>& script) {
		//initilize component
		if (InitManagedGameObject(gameObject, script)) {
			script->CallMethod(m_manager->getEvents().onAwake);
		}
		else {
			CORE_ERROR("Failed to init gameObject");
		}
	}
	bool EventCaller::InitManagedGameObject(const GameObject& gameObject, Ref<MonoObjectInstance>& script) {
		if (m_initManagedMethod) {
			uint64_t id = (uint64_t)gameObject.GetID();
			void* args[1] = { &id };
			script->CallMethod(m_initManagedMethod, args, false);
			script->SetInitilized();
			return true;
		}
		CORE_ERROR("Could not find Stulu.GameObjectAttached::initilize");
		return false;
	}
	void EventCaller::CallManagedEvent(Mono::Method method, const GameObject& object) {
		if (object) {
			ScriptingComponent& comp = object.getComponent<ScriptingComponent>();
			for (auto& script : comp.runtimeScripts) {
				if (!script)
					continue;

				if (!script->IsInitilized()) {
					InitManagedRuntimeScript(object, script);
				}
				if (method) {
					script->CallMethod(method);
				}
			}
			return;
		}
		for (auto& [id, comp] : m_scene->getRegistry().storage<ScriptingComponent>().each()) {
			const GameObject gameObject = { id, m_scene };
			CallManagedEvent(method, gameObject);
		}
	}

#define DEFAULT_HANDLE_MANAGED(name) \
	{\
		ST_PROFILING_SCOPE("Managed Scripting - " #name); \
		if (object.IsValid()) { \
			if (object.hasComponent<ScriptingComponent>()) { \
				CallManagedEvent(m_manager->getEvents().name, object); \
			} \
			return; \
		} \
		for (auto& [id, comp] : m_scene->getRegistry().storage<ScriptingComponent>().each()) { \
			const GameObject gameObject = { id, m_scene }; \
			CallManagedEvent(m_manager->getEvents().name, gameObject); \
		} \
	} 

#define DEFAULT_HANDLE_LAYER(name, ...) \
	{\
		ST_PROFILING_SCOPE("Native Scripting - " #name); \
		for (auto& [id, layer] : m_layer) { \
			layer->name(__VA_ARGS__); \
		} \
	}

	void EventCaller::onAwake(const GameObject& object) {
		DEFAULT_HANDLE_MANAGED(onAwake);
	}
	void EventCaller::onDestroy(const GameObject& object) {
		DEFAULT_HANDLE_MANAGED(onDestroy);
	}

	void EventCaller::onStart(const GameObject& object) {
		DEFAULT_HANDLE_MANAGED(onStart);
	}
	void EventCaller::onUpdate(const GameObject& object) {
		if (object == GameObject::null) {
			DEFAULT_HANDLE_LAYER(Update);
		}

		DEFAULT_HANDLE_MANAGED(onUpdate);
	}
	void EventCaller::onRender(const GameObject& object) {
		if (object == GameObject::null) {
			DEFAULT_HANDLE_LAYER(Render);
		}

		DEFAULT_HANDLE_MANAGED(onRender);
	}
	void EventCaller::onRender2D(const GameObject& object) {
		if (object == GameObject::null) {
			DEFAULT_HANDLE_LAYER(Render2D);
		}

		DEFAULT_HANDLE_MANAGED(onRender2D);
	}
	void EventCaller::onDrawGizmos(const GameObject& object) {
		if (object == GameObject::null) {
			DEFAULT_HANDLE_LAYER(DrawGizmos);
		}

		DEFAULT_HANDLE_MANAGED(onDrawGizmos);
	}
	void EventCaller::onSceneExit(const GameObject& object) {
		if (object == GameObject::null) {
			DEFAULT_HANDLE_LAYER(SceneExit);
		}

		DEFAULT_HANDLE_MANAGED(onSceneExit);
	}
	void EventCaller::NativeSceneStart() {
		DEFAULT_HANDLE_LAYER(SceneStart);
	}
	void EventCaller::NativePreUpdate() {
		DEFAULT_HANDLE_LAYER(PreUpdate);
	}
	void EventCaller::NativeGameObjectCreate(const GameObject& object) {
		DEFAULT_HANDLE_LAYER(GameObjectCreate, object);
	}
	void EventCaller::NativeGameObjectDestory(const GameObject& object) {
		DEFAULT_HANDLE_LAYER(GameObjectDestory, object);
	}
	void EventCaller::SerializerGameObject(YAML::Emitter& out, GameObject& gameObject) {
		DEFAULT_HANDLE_LAYER(SerializerGameObject, out, gameObject);
	}
	void EventCaller::DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path) {
		DEFAULT_HANDLE_LAYER(DeserializerGameObject, gameObject, deserialized, path);
	}
	void EventCaller::SerializerScene(YAML::Emitter& out) {
		DEFAULT_HANDLE_LAYER(SerializerScene, out);
	}
	void EventCaller::DeserializerScene(YAML::Node& data) {
		DEFAULT_HANDLE_LAYER(DeserializerScene, data);
	}
}