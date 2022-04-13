#include "st_pch.h"
#include "AssemblyManager.h"
#include "Stulu/Core/Application.h"

#include "Bindings/Log.h"
#include "Bindings/Time.h"
#include "Bindings/Input.h"
#include "Bindings/GameObject.h"


namespace Stulu {
	AssemblyManager::AssemblyManager(const std::string& assemblyPath, const std::string& monoAssemblyPath, const std::string& monoConfigPath) {
		mono_set_dirs(monoAssemblyPath.c_str(), monoConfigPath.c_str());
		m_monoDomain = mono_jit_init("StuluEngine");
		if (!m_monoDomain) {
			CORE_ERROR("Mono Domain creation failed");
			return;
		}
		loadScriptCore(assemblyPath);
	}

	AssemblyManager::~AssemblyManager() {
		m_assembly.reset();
		m_scriptCoreAssembly.reset();
		if (m_monoDomain) {
			mono_jit_cleanup(m_monoDomain);
		}
	}
	void AssemblyManager::loadScriptCore(const std::string& assemblyPath) {
		m_scriptCoreAssembly = createRef<ScriptAssembly>(m_monoDomain, "data/Stulu.ScriptCore.dll");
		m_assembly = createRef<ScriptAssembly>(m_monoDomain, assemblyPath.c_str());

		MonoClass* componentClass = m_scriptCoreAssembly->createClass("Stulu", "Component");
		m_assembly->loadAllClasses(componentClass);

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
		mono_add_internal_call("Stulu.InternalCalls::time_setScale(single)", StuluBindings::Time::time_setScale);

		mono_add_internal_call("Stulu.InternalCalls::input_keyDown(uint)", StuluBindings::Input::keyDown);
		mono_add_internal_call("Stulu.InternalCalls::input_mouseDown(uint)", StuluBindings::Input::mouseDown);
		mono_add_internal_call("Stulu.InternalCalls::input_setCursorMode(uint)", StuluBindings::Input::setCursorMode);

		mono_add_internal_call("Stulu.InternalCalls::gameObject_add_component(uint,System.Type)", StuluBindings::GameObject::addComponent);
	}
}