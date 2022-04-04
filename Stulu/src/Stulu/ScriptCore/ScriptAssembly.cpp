#include "st_pch.h"
#include "ScriptAssembly.h"

namespace Stulu {
	ScriptAssembly::ScriptAssembly(const std::string& name, const std::string& assembly) {
		CORE_INFO("Loading Assembly: {0}", name);
		m_monoDomain = mono_jit_init(name.c_str());
		if (!m_monoDomain) {
			CORE_ERROR("Mono Domain creation failed for {0}", name);
			return;
		}
		m_monoAssembly = mono_domain_assembly_open(m_monoDomain, assembly.c_str());
		if (!m_monoAssembly) {
			CORE_ERROR("Mono Assembly creation failed for {0}", assembly);
			return;
		}
		m_monoImage = mono_assembly_get_image(m_monoAssembly);
		if (!m_monoImage) {
			CORE_ERROR("Mono Image creation failed for {0}", assembly);
			return;
		}
		m_errorCallBack = [=](const std::string& msg, const MonoFunction& func) {
			CORE_ERROR("C# Runtime Error:\n{0}\nCaused in function: {1}", msg, mono_method_full_name(func.methodPtr, 1));//default error callback
		};
		CORE_INFO("Loading Assembly finished")
	}

	ScriptAssembly::~ScriptAssembly() {
		if (m_monoDomain) {
			mono_jit_cleanup(m_monoDomain);
		}
	}

	MonoClass* ScriptAssembly::createClass(const std::string& m_nameSpace, const std::string& m_className) const {
		MonoClass* classPtr = mono_class_from_name(m_monoImage, m_nameSpace.c_str(), m_className.c_str());
		if (classPtr) {
			return classPtr;
		}
		CORE_ERROR("Could not create MonoClass* from {0}.{1}", m_nameSpace, m_className);
		return classPtr;
	}

	MonoFunction ScriptAssembly::createFunction(const std::string& m_nameSpace, const std::string& m_className, const std::string& functionName) const {
		MonoFunction function;
		function.name = functionName;

		std::string classStr = (std::string(".") + m_className + functionName);

		function.classPtr = mono_class_from_name(m_monoImage, m_nameSpace.c_str(), m_className.c_str());
		if (function.classPtr) {
			MonoMethodDesc* descPtr = mono_method_desc_new(classStr.c_str(), false);
			if (descPtr) {
				function.methodPtr = mono_method_desc_search_in_class(descPtr, function.classPtr);
				if (function.methodPtr) {
					mono_method_desc_free(descPtr);
					return function;
				}
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}{1}", m_nameSpace, classStr);
		return function;
	}

	MonoFunction ScriptAssembly::createFunction(MonoClass* classPtr, const std::string& functionName) {
		MonoFunction function;
		function.classPtr = classPtr;

		const char* m_nameSpace = mono_class_get_namespace(classPtr);
		const char* m_className = mono_class_get_name(classPtr);

		function.name = functionName;

		std::string classStr = (std::string(".") + m_className + functionName);

		if (function.classPtr) {
			MonoMethodDesc* descPtr = mono_method_desc_new(classStr.c_str(), false);
			if (descPtr) {
				function.methodPtr = mono_method_desc_search_in_class(descPtr, function.classPtr);
				if (function.methodPtr) {
					mono_method_desc_free(descPtr);
					return function;
				}
			}
		}
		CORE_ERROR("Could not create Monofunction from {0}{1}", m_nameSpace, classStr);
		return function;
	}

	MonoObject* ScriptAssembly::invokeFunction(const MonoFunction& function, void* obj, void** args) const {
		MonoObject* ex = nullptr;
		MonoObject* re = mono_runtime_invoke(function.methodPtr, obj, args, &ex);
		
		if (ex) {
			MonoString* excM = mono_object_to_string(ex, nullptr);
			m_errorCallBack(mono_string_to_utf8(excM), function);
		}

		return re;
	}

}