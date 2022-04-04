#include "st_pch.h"
#include "AssemblyManager.h"
#include "Stulu/Core/Application.h"

#include "Bindings/Log.h"
#include "Bindings/Time.h"
#include "Bindings/GameObject.h"


namespace Stulu {
	AssemblyManager::AssemblyManager(const std::string& monoAssemblyPath, const std::string& monoConfigPath) {
		mono_set_dirs(monoAssemblyPath.c_str(), monoConfigPath.c_str());
		loadScriptCore();
	}

	AssemblyManager::~AssemblyManager() {
		m_scriptCore.reset();
	}
	int param = 10;
	void AssemblyManager::loadScriptCore() {
		m_scriptCore = createRef<ScriptAssembly>("Stulu-ScriptCore", "data/Stulu-ScriptCore.dll");

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
	}
}