#include "st_pch.h"
#include "AssemblyManager.h"
#include "Stulu/Core/Application.h"

#include "Bindings/Log.h"
#include "Bindings/Time.h"
#include "Bindings/Input.h"
#include "Bindings/GameObject.h"
#include "Bindings/Transform.h"
#include "Bindings/Rigidbody.h"


namespace Stulu {
	AssemblyManager::AssemblyManager(const std::string& assemblyPath, const std::string& coreAssemblyPath, const std::string& monoAssemblyPath, const std::string& monoConfigPath) {
		mono_set_dirs(monoAssemblyPath.c_str(), monoConfigPath.c_str());
		m_monoDomain = mono_jit_init("StuluEngine");
		if (!m_monoDomain) {
			CORE_ERROR("Mono Domain creation failed");
			return;
		}
		loadScriptCore(assemblyPath, coreAssemblyPath);
	}

	AssemblyManager::~AssemblyManager() {
		m_scriptCoreAssembly->m_monoRootDomain = nullptr;
		m_assembly->m_monoRootDomain = nullptr;
		if (m_monoDomain) {
			mono_jit_cleanup(m_monoDomain);
		}
	}
	void AssemblyManager::loadScriptCore(const std::string& assemblyPath, const std::string& coreAssemblyPath) {
		m_scriptCoreAssembly = createRef<ScriptAssembly>(m_monoDomain, coreAssemblyPath);
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

		mono_add_internal_call("Stulu.InternalCalls::input_getKeyDown(uint)", StuluBindings::Input::getKeyDown);
		mono_add_internal_call("Stulu.InternalCalls::input_getKeyUp(uint)", StuluBindings::Input::getKeyUp);
		mono_add_internal_call("Stulu.InternalCalls::input_getKey(uint)", StuluBindings::Input::getKey);
		mono_add_internal_call("Stulu.InternalCalls::input_getMouseDown(uint)", StuluBindings::Input::getMouseDown);
		mono_add_internal_call("Stulu.InternalCalls::input_getMouseUp(uint)", StuluBindings::Input::getMouseUp);
		mono_add_internal_call("Stulu.InternalCalls::input_getMouse(uint)", StuluBindings::Input::getMouse);
		mono_add_internal_call("Stulu.InternalCalls::input_setCursorMode(uint)", StuluBindings::Input::setCursorMode);
		mono_add_internal_call("Stulu.InternalCalls::input_getCursorMode()", StuluBindings::Input::getCursorMode);
		mono_add_internal_call("Stulu.InternalCalls::input_getMouseDelta(Stulu.Vector2&)", StuluBindings::Input::getMouseDelta);
		mono_add_internal_call("Stulu.InternalCalls::input_getMousePos(Stulu.Vector2&)", StuluBindings::Input::getMousePos);

		mono_add_internal_call("Stulu.InternalCalls::gameObject_create(string,string,Stulu.Vector3,Stulu.Quaternion,Stulu.Vector3);", StuluBindings::GameObject::create);
		mono_add_internal_call("Stulu.InternalCalls::gameObject_add_component(uint,System.Type)", StuluBindings::GameObject::addComponent);
		mono_add_internal_call("Stulu.InternalCalls::gameObject_has_component(uint,System.Type)", StuluBindings::GameObject::hasComponent);
		mono_add_internal_call("Stulu.InternalCalls::gameObject_remove_component(uint,System.Type)", StuluBindings::GameObject::removeComponent);
		mono_add_internal_call("Stulu.InternalCalls::gameObject_getName(uint)", StuluBindings::GameObject::getName);
		mono_add_internal_call("Stulu.InternalCalls::gameObject_getTag(uint)", StuluBindings::GameObject::getTag);
		mono_add_internal_call("Stulu.InternalCalls::gameObject_setName(uint,string)", StuluBindings::GameObject::setName);
		mono_add_internal_call("Stulu.InternalCalls::gameObject_setTag(uint,string)", StuluBindings::GameObject::setTag);

		mono_add_internal_call("Stulu.InternalCalls::transformComp_setPos(uint,Stulu.Vector3&)", StuluBindings::Transform::setPos);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_getPos(uint,Stulu.Vector3&)", StuluBindings::Transform::getPos);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_getWorldPos(uint,Stulu.Vector3&)", StuluBindings::Transform::getWorldPos);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_setRotation(uint,Stulu.Quaternion&)", StuluBindings::Transform::setRotation);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_getRotation(uint,Stulu.Quaternion&)", StuluBindings::Transform::getRotation);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_getRotation(uint,Stulu.Vector3&)", StuluBindings::Transform::getRotationInDegree);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_getWorldRotation(uint,Stulu.Quaternion&)", StuluBindings::Transform::getWorldRotation);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_setScale(uint,Stulu.Vector3&)", StuluBindings::Transform::setScale);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_getScale(uint,Stulu.Vector3&)", StuluBindings::Transform::getScale);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_getWorldScale(uint,Stulu.Vector3&)", StuluBindings::Transform::getWorldScale);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_forward(uint,Stulu.Vector3&)", StuluBindings::Transform::forward);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_right(uint,Stulu.Vector3&)", StuluBindings::Transform::right);
		mono_add_internal_call("Stulu.InternalCalls::transformComp_up(uint,Stulu.Vector3&)", StuluBindings::Transform::up);

		mono_add_internal_call("Stulu.InternalCalls::rigidbody_addForce(uint,Stulu.Vector3&,uint)", StuluBindings::Rigidbody::addForce);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_useGravity(uint)", StuluBindings::Rigidbody::getuseGravity);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_kinematic(uint)", StuluBindings::Rigidbody::getKinematic);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_retainaccel(uint)", StuluBindings::Rigidbody::getRetainAccel);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_mass(uint)", StuluBindings::Rigidbody::getMass);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_useGravity(uint,bool)", StuluBindings::Rigidbody::setuseGravity);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_kinematic(uint,bool)", StuluBindings::Rigidbody::setKinematic);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_retainaccel(uint,bool)", StuluBindings::Rigidbody::setRetainAccel);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_mass(uint,single)", StuluBindings::Rigidbody::setMass);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_massCenterSet(uint,Stulu.Vector3&)", StuluBindings::Rigidbody::setMassCenterPos);
		mono_add_internal_call("Stulu.InternalCalls::rigidbody_massCenterGet(uint,Stulu.Vector3&)", StuluBindings::Rigidbody::getMassCenterPos);

	}
}
