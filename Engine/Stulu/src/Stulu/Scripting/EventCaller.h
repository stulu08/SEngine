#pragma once
#include "Stulu/Scene/GameObject.h"
#include "Managed/AssemblyManager.h"

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

		void ConstructManaged(const GameObject& object = GameObject::null);
		void InitManagedRuntimeScript(const GameObject& gameObject, Ref<MonoObjectInstance>& script);
		bool InitManagedGameObject(const GameObject& gameObject, Ref<MonoObjectInstance>& script);

		void CallManagedEvent(Mono::Method method, const GameObject& object = GameObject::null);

		void ConstructNative(const GameObject& object = GameObject::null);
		void DestructNative(const GameObject& object = GameObject::null);
	private:
		Scene* m_scene;
		Ref<AssemblyManager> m_manager;
	};
}