#pragma once
#include "MonoObjectInstance.h"

#include "Bindings/Log.h"
#include "Bindings/Time.h"
#include "Bindings/Input.h"
#include "Bindings/GameObject.h"
#include "Bindings/Transform.h"
#include "Bindings/GameObject.h"
#include "Bindings/Rigidbody.h"
#include "Bindings/Graphics2D.h"
#include "Bindings/Texture2D.h"
#include "Bindings/Gizmo.h"
#include "Bindings/Folders.h"
#include "Bindings/SpriteRenderer.h"

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
			StuluBindings::GameObject::addComponentRegister[managedName] = [](GameObject go) { go.addComponent<T>(); };
			StuluBindings::GameObject::hasComponentRegister[managedName] = [](GameObject go) { return go.hasComponent<T>(); };
			StuluBindings::GameObject::removeComponentRegister[managedName] = [](GameObject go) { return go.removeComponent<T>(); };
		}
		template<class T>
		void RegisterProperty(const std::string& managedName) {
			Property::TypeRegister[managedName] = [](MonoObject* object, MonoClassField* field) { return createRef<T>(object, field); };
		}
		// eg "NameSpace.Class::Function(uint,string,single)"
		void RegisterFunction(const std::string& fullManagedStaticFuncName, const void* func);
	private:
		void loadScriptCore(const std::string& assemblyPath, const std::string& coreAssemblyPath);

		Ref<ScriptAssembly> m_assembly = nullptr;
		Ref<ScriptAssembly> m_scriptCoreAssembly = nullptr;
		MonoDomain* m_monoDomain = nullptr;
	};
}