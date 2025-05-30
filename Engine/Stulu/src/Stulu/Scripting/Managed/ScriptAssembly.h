#pragma once
#include "Mono.h"
#include "Stulu/Types/UUID.h"

namespace Stulu {
	class STULU_API MonoObjectInstance;
	class STULU_API ScriptAssembly {
	public:
		inline ScriptAssembly(const std::string& assembly, bool loadDebugSymboles = false) {
			Load(assembly, loadDebugSymboles);
		}
		inline ~ScriptAssembly() {
			Unload();
		}

		void Load(const std::string& assembly, bool loadDebugSymboles = false);
		void Unload();

		std::vector<Mono::Class> LoadAllClasses(Mono::Class parentClass);

		Mono::Class CreateClass(const std::string& nameSpace, const std::string& className) const;

		// methodName format: Test(string,int)
		inline Mono::Method CreateMethod(const std::string& classNameSpace, const std::string& className, const std::string& methodName) const {
			return CreateMethod(Mono::Class::FromName(m_image, classNameSpace, className), methodName);
		}
		static Mono::Method CreateMethod(Mono::Class classPtr, const std::string& methodnName);

		Mono::Object InvokeMethod(Mono::Method method, void* obj, void** args);

		void RegisterObject(UUID id, MonoObjectInstance* object) { m_objects.insert({ id, object }); }
		void DeRegisterObject(UUID id) { m_objects.erase(id); }

		Mono::Assembly getAssembly() const { return m_assembly; }
		Mono::Image getImage() const { return m_image; }
	private:
		void LoadAssembly(const std::string& assembly, bool loadDebugSymboles = false);

		Mono::Assembly m_assembly = nullptr;
		Mono::Image m_image = nullptr;

		std::unordered_map<Stulu::UUID, MonoObjectInstance*> m_objects;

		friend class AssemblyManager;
	};
}