#pragma once
#include "Stulu/Core/Core.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/object.h>

namespace Stulu {
	struct MonoFunction {
		MonoClass* classPtr;
		MonoMethod* methodPtr;

		std::string name = "";
	};
	class STULU_API MonoObjectInstance {
	public:
		MonoObjectInstance() {};
		MonoObjectInstance(const std::string& nameSpace, const std::string& className);
		~MonoObjectInstance();

		void addFunction(const std::string& fnName, const MonoFunction& mfn);

		void callConstructor(const std::string& params, void** args = nullptr);
		MonoObject* call(const std::string& func, void** args = NULL, bool isStatic = false);
	private:
		void load();

		uint32_t m_gCHandle = 0;//to prevent garbage collection
		MonoClass* classPtr = nullptr;
		MonoObject* objectPtr = nullptr;
		//function name: print(int)
		std::unordered_map<std::string, MonoFunction> functions;
		std::string nameSpace = "", className = "";
	};
}