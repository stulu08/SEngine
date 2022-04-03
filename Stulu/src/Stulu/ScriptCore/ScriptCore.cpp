#include "st_pch.h"
#include "ScriptCore.h"
#include "Stulu/Core/Application.h"

#include "Bindings/Log.h"
#include "Bindings/Time.h"
#include "Bindings/GameObject.h"


namespace Stulu {
	ScriptCore::ScriptCore(const std::string& monoAssemblyPath, const std::string& monoConfigPath) {
		s_instance = this;
		mono_set_dirs(monoAssemblyPath.c_str(), monoConfigPath.c_str());
		load();
	}

	ScriptCore::~ScriptCore() {
		if (m_monoScriptCoreDomain) {
			mono_jit_cleanup(m_monoScriptCoreDomain);
		}
	}

	void ScriptCore::load() {
		CORE_INFO("Loading ScriptCore")
			m_monoScriptCoreDomain = mono_jit_init(SCRIPT_CORE_NAME);
		if (!m_monoScriptCoreDomain) {
			CORE_ERROR("Mono Domain creation failed for {0}", SCRIPT_CORE_NAME);
			return;
		}
		m_monoScriptCoreAssembly = mono_domain_assembly_open(m_monoScriptCoreDomain, SCRIPT_CORE_DLL_PATH);
		if (!m_monoScriptCoreAssembly) {
			CORE_ERROR("Mono Assembly creation failed for {0}", SCRIPT_CORE_DLL_PATH);
			return;
		}
		m_monoScriptCoreImage = mono_assembly_get_image(m_monoScriptCoreAssembly);
		if (!m_monoScriptCoreImage) {
			CORE_ERROR("Mono Image creation failed for {0}", SCRIPT_CORE_DLL_PATH);
			return;
		}
		addInternalCalls();
		CORE_INFO("Loading ScriptCore finished")
	}
	int param = 10;
	void ScriptCore::addInternalCalls() {
		mono_add_internal_call("Stulu.InternalCalls::application_exit(int)", Application::exit);
		mono_add_internal_call("Stulu.InternalCalls::application_getWidth()", Application::getWidth);
		mono_add_internal_call("Stulu.InternalCalls::application_getHeight()", Application::getHeight);
		mono_add_internal_call("Stulu.InternalCalls::log_engine_log(int,string)", StuluBindings::Log::engine_log);
		mono_add_internal_call("Stulu.InternalCalls::log_client_log(int,string)", StuluBindings::Log::client_log);

		mono_add_internal_call("Stulu.InternalCalls::time_getFrameTime()", StuluBindings::Time::time_frame);
		mono_add_internal_call("Stulu.InternalCalls::time_getDeltaTime()", StuluBindings::Time::time_delta);
		mono_add_internal_call("Stulu.InternalCalls::time_getApplicationRuntime()", StuluBindings::Time::time_runtime);
		mono_add_internal_call("Stulu.InternalCalls::time_getScale()", StuluBindings::Time::time_scale);
		mono_add_internal_call("Stulu.InternalCalls::time_getTime()", StuluBindings::Time::time_time);

		mono_add_internal_call("Stulu.InternalCalls::gameObject_add_component(uint,System.Type)", StuluBindings::GameObject::addComponent);
		

		m_engineObject = createRef<MonoObjectInstance>("Stulu", "Engine");
	}


	MonoClass* ScriptCore::createClass(const std::string& nameSpace, const std::string& className) {
		MonoClass* classPtr = mono_class_from_name(m_monoScriptCoreImage, nameSpace.c_str(), className.c_str());
		if (classPtr) {
			return classPtr;
		}
		CORE_ERROR("Could not create MonoClass* from {0}.{1}", nameSpace, className);
		return classPtr;
	}
	MonoFunction ScriptCore::createFunction(const std::string& nameSpace, const std::string& className, const std::string& functionName) {
		MonoFunction function;
		function.name = functionName;

		std::string classStr = (std::string(".") + className + functionName);

		function.classPtr = mono_class_from_name(m_monoScriptCoreImage, nameSpace.c_str(), className.c_str());
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
		CORE_ERROR("Could not create Monofunction from {0}{1}", nameSpace, classStr);
		return function;
	}
	MonoFunction ScriptCore::createFunction(MonoClass* classPtr, const std::string& functionName) {
		MonoFunction function;
		function.classPtr = classPtr;

		const char* nameSpace = mono_class_get_namespace(classPtr);
		const char* className = mono_class_get_name(classPtr);

		function.name = functionName;

		std::string classStr = (std::string(".") + className + functionName);

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
		CORE_ERROR("Could not create Monofunction from {0}{1}", nameSpace, classStr);
		return function;
	}
	MonoObject* ScriptCore::invokeFunction(const MonoFunction& function,void* obj, void** args) {
		MonoObject* object = NULL;
		MonoObject* re = mono_runtime_invoke(function.methodPtr, obj, args, &object);
		return re;
	}
}