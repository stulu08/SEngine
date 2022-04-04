#include "st_pch.h"
#include "MonoObjectInstance.h"

#include "AssemblyManager.h"

namespace Stulu {
	MonoObjectInstance::MonoObjectInstance(const std::string& nameSpace, const std::string& className, const ScriptAssembly* assembly)
		:m_nameSpace(nameSpace),m_className(className), m_assembly(assembly) {
		m_classPtr = m_assembly->createClass(m_nameSpace, m_className);
		if (m_classPtr) {
			load();
			m_objectPtr = mono_object_new(m_assembly->getDomain(), m_classPtr);
			m_gCHandle = mono_gchandle_new(m_objectPtr, false);
			return;
		}
		CORE_ERROR("Could not create MonoObjectInstance from {0}{1}", m_nameSpace, m_className);
	}

	MonoObjectInstance::~MonoObjectInstance() {
		if (m_gCHandle) {
			mono_gchandle_free(m_gCHandle);
			m_gCHandle = 0;
		}
	}

	void MonoObjectInstance::addFunction(const std::string& fnName, const MonoFunction& mfn) {
		m_functions[fnName] = mfn;
	}

	void MonoObjectInstance::callConstructor(const std::string& params, void** args) const {
		call((std::string(".ctor") + params), args, false);
	}

	MonoObject* MonoObjectInstance::call(const std::string& func, void** args, bool isStatic) const {
		const std::string name = m_nameSpace + "." + m_className + ":" + func;
		if (m_functions.find(name) != m_functions.end()) {
			return m_assembly->invokeFunction(m_functions.at(name), isStatic ? NULL : m_objectPtr, args);
		}
		CORE_ERROR("Function not found: {0}", name);
		return nullptr;
	}
	void MonoObjectInstance::load() {
		void* iter = NULL;
		MonoMethod* method;
		while (method = mono_class_get_methods(m_classPtr, &iter)) {
			MonoFunction func;
			func.name = mono_method_full_name(method, 1);
			if (func.name.find_first_of(" (") != func.name.npos)
				func.name.replace(func.name.find_first_of(" ("), 1, "");//remove space befor the parameters "exampleFunc (uint,string)" -> "exampleFunc(uint,string)"

			MonoMethodDesc* desc = mono_method_desc_new(func.name.c_str(), true);
			if (desc) {
				func.methodPtr = mono_method_desc_search_in_class(desc, m_classPtr);
				mono_method_desc_free(desc);
			}
			func.classPtr = m_classPtr;
			
			m_functions[func.name] = func;
		}
	}
}