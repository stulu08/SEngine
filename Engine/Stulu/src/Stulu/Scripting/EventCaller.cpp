#include "st_pch.h"
#include "EventCaller.h"

#include "Stulu/Core/Application.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/Behavior.h"

namespace Stulu {
	EventCaller::EventCaller(Scene* scene)
		: m_scene(scene) {
		m_manager = Application::get().getAssemblyManager();
	}
#define IMPL_EVENT(name) \
	void EventCaller::name(const GameObject& object) { \
		if (object) { \
			if (object.hasComponent<NativeScriptComponent>()) { \
				NativeScriptComponent& script = object.getComponent<NativeScriptComponent>(); \
				if (!script.instance) { \
					ConstructNative(object); \
				} \
				if (script.instance) { \
					script.instance->name(); \
				} \
			} \
			if (object.hasComponent<ScriptingComponent>()) { \
				CallManagedEvent(m_manager->getEvents().name, object); \
			} \
			return; \
		} \
		m_scene->m_registry.view<NativeScriptComponent>().each([this](auto goID, NativeScriptComponent& script) { \
			if (!script.instance) { \
				ConstructNative({ goID , m_scene}); \
			} \
			if (script.instance) { \
				script.instance->onAwake(); \
			} \
		}); \
		for (auto& [id, comp] : m_scene->m_registry.storage<ScriptingComponent>().each()) { \
			const GameObject gameObject = { id, m_scene }; \
			CallManagedEvent(m_manager->getEvents().name, gameObject); \
		} \
	}
	IMPL_EVENT(onAwake);
	IMPL_EVENT(onStart);
	IMPL_EVENT(onUpdate);
	IMPL_EVENT(onRender);
	IMPL_EVENT(onRender2D);
	IMPL_EVENT(onDrawGizmos);
	IMPL_EVENT(onDestroy);
	IMPL_EVENT(onSceneExit);

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
	void EventCaller::ConstructNative(const GameObject& object) {
		if (object) {
			NativeScriptComponent& script = object.getComponent<NativeScriptComponent>();
			script.instance = script.Instantiate();
			script.instance->gameObject = object;
			script.instance->onAwake();
			return;
		}
		m_scene->m_registry.view<NativeScriptComponent>().each([=](auto gameObject, NativeScriptComponent& script) {
			script.instance = script.Instantiate();
			script.instance->gameObject = GameObject{ gameObject, m_scene };
			script.instance->onAwake();
			});
	}
	void EventCaller::DestructNative(const GameObject& object) {
		if (object) {
			NativeScriptComponent& script = object.getComponent<NativeScriptComponent>();
			if (script.instance) {
				script.Destroy(&script);
			}
			return;
		}
		m_scene->m_registry.view<NativeScriptComponent>().each([=](auto gameObject, NativeScriptComponent& script) {
			if (script.instance) {
				script.Destroy(&script);
			}
		});
	}
}