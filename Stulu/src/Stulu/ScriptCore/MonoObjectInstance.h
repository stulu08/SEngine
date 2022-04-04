#pragma once
#include "Stulu/Core/Core.h"

#include "ScriptAssembly.h"


namespace Stulu {
	class STULU_API MonoObjectInstance {
	public:
		MonoObjectInstance(const std::string& m_nameSpace, const std::string& m_className, const ScriptAssembly* assembly);
		~MonoObjectInstance();

		void addFunction(const std::string& fnName, const MonoFunction& mfn);

		void callConstructor(const std::string& params, void** args = nullptr) const;
		MonoObject* call(const std::string& func, void** args = NULL, bool isStatic = false) const;
	private:
		void load();

		uint32_t m_gCHandle = 0;
		MonoClass* m_classPtr = nullptr;
		MonoObject* m_objectPtr = nullptr;

		const ScriptAssembly* m_assembly;
		
		std::string m_nameSpace = "", m_className = "";

		//function name: print(int)
		std::unordered_map<std::string, MonoFunction> m_functions;
	};
}