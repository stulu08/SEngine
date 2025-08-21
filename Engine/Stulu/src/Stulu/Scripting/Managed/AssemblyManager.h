#pragma once
#include "MonoObjectInstance.h"

#include "Mono.h"
#include "Stulu/Scene/GameObject.h"

namespace Stulu {
	class STULU_API AssemblyManager {
	public:
		AssemblyManager(
			const std::string& assemblyPath, const std::string& coreAssemblyPath, bool debugging = false,
			const std::string& monoAssemblyDir = ".", const std::string& monoConfigDir = ".");
		~AssemblyManager();

		void Reload();

		template<class T>
		inline void RegisterComponent(const std::string& managedName) {
			m_addComponentRegister[managedName] = [](GameObject go) { go.addComponent<T>(); };
			m_hasComponentRegister[managedName] = [](GameObject go) { return go.IsValid() && go.hasComponent<T>(); };
			m_removeComponentRegister[managedName] = [](GameObject go) { return go.removeComponent<T>(); };
		}
		template<class T>
		inline void RegisterProperty(const std::string& managedName) {
			m_propertieRegister[managedName] = [](Mono::Object object, Mono::ClassField field) { return createRef<T>(object, field); };
		}
		// eg "NameSpace.Class::Function(uint,string,single)"
		inline void RegisterFunction(const std::string& fullManagedStaticFuncName, const void* func) {
			Mono::AddInternallCall(fullManagedStaticFuncName, func);
		}

		void LoadComponents();

		void ManagedAddComponent(GameObject object, Mono::Class componentChildClass) const;
		bool ManagedHasComponent(GameObject object, Mono::Class componentChildClass) const;
		bool ManagedRemoveComponent(GameObject object, Mono::Class componentChildClass) const;
		Mono::Object ManagedGetComponent(GameObject gameObject, Mono::Class componentChildClass) const;


		inline auto& GetPropertieRegister() { return m_propertieRegister; }
		inline auto& GetComponentRegister_Add() { return m_addComponentRegister; }
		inline auto& GetComponentRegister_Has() { return m_hasComponentRegister; }
		inline auto& GetComponentRegister_Remove() { return m_removeComponentRegister; }

		Ref<ScriptAssembly>& getAppAssembly() { return m_appAssembly; }
		const Ref<ScriptAssembly>& getAppAssembly() const { return m_appAssembly; }
		Ref<ScriptAssembly>& getScriptCoreAssembly() { return m_scriptCoreAssembly; }
		const Ref<ScriptAssembly>& getScriptCoreAssembly() const { return m_scriptCoreAssembly; }

		Mono::Domain getRootDomain() const { return m_monoDomain; }
		Mono::Domain getCoreDomain() const { return m_coreDomain; }

		bool DebugEnabled() const { return m_enabledDebugging; }
		Mono::Method GetStackTraceMethod() const { return m_exceptionStackTraceMethod; }

		Mono::Class getComponentClass() const { return m_componentClass; }
		Mono::Class getGoAttachedClass() const { return m_gameObjectAttachedClass; }
		const auto& getEvents() const { return m_events; }
		inline const std::vector<Mono::Class> GetComponents() const { return m_components; }
	private:
		void InitMono();
		void CreateAppDomain();
		void LoadAssemblies();
		void LoadDefaults();

		Ref<ScriptAssembly> m_appAssembly = nullptr;
		Ref<ScriptAssembly> m_scriptCoreAssembly = nullptr;
		std::filesystem::path m_corePath, m_appPath;
		bool m_enabledDebugging;

		Mono::Domain m_monoDomain = nullptr;
		Mono::Domain m_coreDomain = nullptr;

		Mono::Class m_componentClass = nullptr;
		Mono::Class m_gameObjectAttachedClass = nullptr;
		struct ManagedEvents {
			Mono::Method onAwake = nullptr;
			Mono::Method onStart = nullptr;
			Mono::Method onUpdate = nullptr;
			Mono::Method onRender = nullptr;
			Mono::Method onRender2D = nullptr;
			Mono::Method onDrawGizmos = nullptr;
			Mono::Method onDestroy = nullptr;
			Mono::Method onSceneExit = nullptr;
		} m_events;
		std::vector<Mono::Class> m_components;

		Mono::Method m_exceptionStackTraceMethod = nullptr;

		std::unordered_map<std::string, std::function<Ref<ManagedProperty>(Mono::Object object, Mono::ClassField field)>> m_propertieRegister;
		std::unordered_map<std::string, std::function<void(GameObject)>> m_addComponentRegister;
		std::unordered_map<std::string, std::function<bool(GameObject)>> m_hasComponentRegister;
		std::unordered_map<std::string, std::function<bool(GameObject)>> m_removeComponentRegister;
	};
}