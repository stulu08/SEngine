#pragma once
#include "MonoObjectInstance.h"
#include "Stulu/Scene/GameObject.h"

namespace Stulu {
	class STULU_API AssemblyManager {
	public:
		AssemblyManager(const std::string& assemblyPath, const std::string& coreAssemblyPath, const std::string& monoAssemblyDir = ".", const std::string& monoConfigDir = ".");
		~AssemblyManager();

		const Ref<ScriptAssembly>& getAssembly() { return m_assembly; }
		const Ref<ScriptAssembly>& getScriptCoreAssembly() { return m_scriptCoreAssembly; }
		MonoDomain* getDomain() const { return m_monoDomain; }

		template<class T>
		void RegisterComponent(const std::string& managedName) {
			m_addComponentRegister[managedName] = [](GameObject go) { go.addComponent<T>(); };
			m_hasComponentRegister[managedName] = [](GameObject go) { return go.hasComponent<T>(); };
			m_removeComponentRegister[managedName] = [](GameObject go) { return go.removeComponent<T>(); };
		}
		template<class T>
		void RegisterProperty(const std::string& managedName) {
			Property::TypeRegister[managedName] = [](MonoObject* object, MonoClassField* field) { return createRef<T>(object, field); };
		}
		// eg "NameSpace.Class::Function(uint,string,single)"
		void RegisterFunction(const std::string& fullManagedStaticFuncName, const void* func);

		inline auto& GetComponentRegister_Add() { return m_addComponentRegister; }
		inline auto& GetComponentRegister_Has() { return m_hasComponentRegister; }
		inline auto& GetComponentRegister_Remove() { return m_removeComponentRegister; }
	private:
		void loadScriptCore(const std::string& assemblyPath, const std::string& coreAssemblyPath);

		Ref<ScriptAssembly> m_assembly = nullptr;
		Ref<ScriptAssembly> m_scriptCoreAssembly = nullptr;
		MonoDomain* m_monoDomain = nullptr;

		std::unordered_map<std::string, std::function<void(GameObject)>> m_addComponentRegister;
		std::unordered_map<std::string, std::function<bool(GameObject)>> m_hasComponentRegister;
		std::unordered_map<std::string, std::function<bool(GameObject)>> m_removeComponentRegister;
	};
}