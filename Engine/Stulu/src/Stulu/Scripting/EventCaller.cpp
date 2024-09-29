#include "st_pch.h"
#include "EventCaller.h"

#include "Stulu/Core/Application.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {

	std::unordered_map<size_t, std::function<SceneLayer* (Scene*)>> EventCaller::s_registeredSceneLayers;


	EventCaller::EventCaller(Scene* scene)
		: m_scene(scene) {

		for (auto& [id, func] : s_registeredSceneLayers) {
			m_layer.push(func(scene));
		}

		m_manager = Application::get().getAssemblyManager();
	}

	void EventCaller::ConstructManaged(const GameObject& object) {
		if (object) {
			ScriptingComponent& comp = object.getComponent<ScriptingComponent>();
			for (auto& script : comp.runtimeScripts) {
				InitManagedRuntimeScript(object, script);
			}
			return;
		}

		for (auto& [id, comp] : m_scene->m_registry.storage<ScriptingComponent>().each()) {
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
		Mono::Method func = m_manager->getGoAttachedClass().GetMethodFromName("initilize", 1);
		if (func) {
			entt::entity id = (entt::entity)gameObject;
			void* args[1];
			args[0] = &id;
			m_manager->getAppAssembly()->InvokeMethod(func, script->getObject(), args);
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
				if (!script->IsInitilized()) {
					InitManagedRuntimeScript(object, script);
				}
				if (method) {
					script->CallMethod(method);
				}
			}
			return;
		}
		for (auto& [id, comp] : m_scene->m_registry.storage<ScriptingComponent>().each()) {
			const GameObject gameObject = { id, m_scene };
			CallManagedEvent(method, gameObject);
		}
	}

#define DEFAULT_HANDLE_MANAGED(name) \
	if (object) { \
		if (object.hasComponent<ScriptingComponent>()) { \
			CallManagedEvent(m_manager->getEvents().name, object); \
		} \
		return; \
	} \
	for (auto& [id, comp] : m_scene->m_registry.storage<ScriptingComponent>().each()) { \
		const GameObject gameObject = { id, m_scene }; \
		CallManagedEvent(m_manager->getEvents().name, gameObject); \
	}
#define DEFAULT_HANDLE_LAYER(name, ...) \
	for (auto layer : m_layer) { \
		layer->name(__VA_ARGS__); \
	}

	void EventCaller::onAwake(const GameObject& object) {
		DEFAULT_HANDLE_MANAGED(onAwake);
	}
	void EventCaller::onDestroy(const GameObject& object) {
		DEFAULT_HANDLE_MANAGED(onDestroy);
	}

	void EventCaller::onStart(const GameObject& object) {
		if (object == GameObject::null) {
			DEFAULT_HANDLE_LAYER(Start);
		}

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
	void EventCaller::GameObjectCreate(const GameObject& object) {
		DEFAULT_HANDLE_LAYER(GameObjectCreate, object);
	}
	void EventCaller::GameObjectDestory(const GameObject& object) {
		DEFAULT_HANDLE_LAYER(GameObjectDestory, object);
	}
	void EventCaller::SerializerGameObject(YAML::Emitter& out, GameObject& gameObject) {
		DEFAULT_HANDLE_LAYER(SerializerGameObject, out, gameObject);
	}
	void EventCaller::DeserializerGameObject(YAML::detail::iterator_value& gameObject, GameObject& deserialized, const std::string& path) {
		DEFAULT_HANDLE_LAYER(DeserializerGameObject, gameObject, deserialized, path);
	}
}