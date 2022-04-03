#include "st_pch.h"
#include "MonoObjectInstance.h"

#include "ScriptCore.h"

namespace Stulu {
	MonoObjectInstance::MonoObjectInstance(const std::string& _nameSpace, const std::string& _className) 
		:nameSpace(_nameSpace),className(_className) {
		classPtr = mono_class_from_name(ScriptCore::get()->m_monoScriptCoreImage, nameSpace.c_str(), className.c_str());
		if (classPtr) {
			load();
			objectPtr = mono_object_new(ScriptCore::get()->m_monoScriptCoreDomain, classPtr);
			m_gCHandle = mono_gchandle_new(objectPtr, false);
			return;
		}
		CORE_ERROR("Could not create MonoObjectInstance from {0}{1}", nameSpace, className);
	}

	MonoObjectInstance::~MonoObjectInstance() {
		if (m_gCHandle) {
			mono_gchandle_free(m_gCHandle);
			m_gCHandle = 0;
		}
	}

	void MonoObjectInstance::addFunction(const std::string& fnName, const MonoFunction& mfn) {
		functions[fnName] = mfn;
	}

	void MonoObjectInstance::callConstructor(const std::string& params, void** args) {
		call((std::string(".ctor") + params), args, false);
	}

	MonoObject* MonoObjectInstance::call(const std::string& func, void** args, bool isStatic) {
		const std::string name = nameSpace + "." + className + ":" + func;
		if (functions.find(name) != functions.end()) {
			MonoObject* object = NULL;
			MonoObject* re = mono_runtime_invoke(functions[name].methodPtr, isStatic ? NULL : objectPtr, args, &object);
			return re;
		}
		CORE_ERROR("Function not found: {0}", name);
		return nullptr;
	}
	void MonoObjectInstance::load() {
		void* iter = NULL;
		MonoMethod* method;
		while (method = mono_class_get_methods(classPtr, &iter)) {
			MonoFunction func;

			func.name = mono_method_full_name(method, 1);
			if (func.name.find_first_of(" (") != func.name.npos)
				func.name.replace(func.name.find_first_of(" ("), 1, "");//remove space befor the parameters "exampleFunc (uint,string)" -> "exampleFunc(uint,string)"

			MonoMethodDesc* desc = mono_method_desc_new(func.name.c_str(), true);
			if (desc) {
				func.methodPtr = mono_method_desc_search_in_class(desc,classPtr);
				mono_method_desc_free(desc);
			}
			//func.methodPtr = method;
			func.classPtr = classPtr;

			
			functions[func.name] = func;
			//addFunction(func.name, func);
		}
	}
}