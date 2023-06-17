#include "st_pch.h"
#include "AssemblyManager.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Scene/YAML.h"


#define StuluTypeRegister(_Type, DefinedType, TypeName) \
{ \
	Stulu::MonoClassMemberTypeFnInfo info; \
	info.type = MonoObjectInstance::MonoClassMember::Type::DefinedType; \
	info.getSize = []() -> size_t { \
		return sizeof(_Type); \
	}; \
	info.assignFieldValue = [](MonoClassField* src, MonoObject* object) -> void* { \
		return Stulu::assignFieldValue<_Type>(src, object); \
	}; \
	info.updateFieldValue = [](Stulu::MonoObjectInstance::MonoClassMember& src, MonoObject* object) -> void* { \
		return Stulu::updateFieldValue<_Type>(src.value, src.m_fieldPtr, object); \
	}; \
	info.serialize = [](YAML::Emitter& out, Stulu::MonoObjectInstance::MonoClassMember& field) { \
		out << YAML::Key << "Value" << *((_Type*)field.value); \
	}; \
	info.deserialize = [](std::unordered_map<std::string, Stulu::MonoObjectInstance::MonoClassMember>& fieldList, std::string name, YAML::detail::iterator_value& field) { \
		*((_Type*)fieldList[name].value) = field["Value"].as<_Type>(); \
	}; \
	/*defined in EditorBindings*/ \
	info.uiFunc = [](Stulu::Ref<Stulu::MonoObjectInstance>& script, Stulu::MonoObjectInstance::MonoClassMember& field, const std::string& name) { }; \
	Stulu::MonoClassMemberTypeFnInfo::infos[TypeName] = info; \
}

namespace Stulu {
	AssemblyManager::AssemblyManager(const std::string& assemblyPath, const std::string& coreAssemblyPath, const std::string& monoAssemblyPath, const std::string& monoConfigPath) {
		ST_PROFILING_FUNCTION();
		mono_set_dirs(monoAssemblyPath.c_str(), monoConfigPath.c_str());
		m_monoDomain = mono_jit_init("StuluEngine");
		if (!m_monoDomain) {
			CORE_ERROR("Mono Domain creation failed");
			return;
		}

		StuluTypeRegister(glm::vec4, Vector4_t, "Stulu.Vector4");
		StuluTypeRegister(glm::vec3, Vector3_t, "Stulu.Vector4");
		StuluTypeRegister(glm::vec2, Vector2_t, "Stulu.Vector4");
		StuluTypeRegister(float, float_t, "System.Single");
		StuluTypeRegister(uint32_t, uint_t, "System.UInt32");
		StuluTypeRegister(int32_t, int_t, "System.Int32");
		StuluTypeRegister(bool, bool_t, "System.Boolean");

		loadScriptCore(assemblyPath, coreAssemblyPath);
	}

	AssemblyManager::~AssemblyManager() {
		ST_PROFILING_FUNCTION();
		m_scriptCoreAssembly->m_monoRootDomain = nullptr;
		m_assembly->m_monoRootDomain = nullptr;
		if (m_monoDomain) {
			mono_jit_cleanup(m_monoDomain);
		}
	}
}



#include "Bindings/Log.h"
#include "Bindings/Time.h"
#include "Bindings/Input.h"
#include "Bindings/GameObject.h"
#include "Bindings/Transform.h"
#include "Bindings/Rigidbody.h"
#include "Bindings/Graphics2D.h"
#include "Bindings/Gizmo.h"

#define add_call(Name, _Binding) mono_add_internal_call((std::string("Stulu.InternalCalls::") + Name).c_str(), StuluBindings::_Binding)
#define _add_call(Name, _Binding) mono_add_internal_call((std::string("Stulu.InternalCalls::") + Name).c_str(), _Binding)

namespace Stulu {
	void AssemblyManager::loadScriptCore(const std::string& assemblyPath, const std::string& coreAssemblyPath) {
		ST_PROFILING_FUNCTION();
		m_scriptCoreAssembly = createRef<ScriptAssembly>(m_monoDomain, coreAssemblyPath);
		m_assembly = createRef<ScriptAssembly>(m_monoDomain, assemblyPath.c_str());

		MonoClass* componentClass = m_scriptCoreAssembly->createClass("Stulu", "Component");
		m_assembly->loadAllClasses(componentClass);

		StuluGameObject_RegisterComponent("Stulu.TransformComponent", TransformComponent);
		StuluGameObject_RegisterComponent("Stulu.RigidbodyComponent", RigidbodyComponent);

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
		add_call("gameObject_createSphere(string,string,Stulu.Vector3)", GameObject::createSphere);
		add_call("gameObject_createCube(string,string,Stulu.Vector3)", GameObject::createCube);
		add_call("gameObject_createCapsule(string,string,Stulu.Vector3)", GameObject::createCapsule);
		add_call("gameObject_createPlane(string,string,Stulu.Vector3)", GameObject::createPlane);

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

		add_call("renderer2D_drawLine(Stulu.Vector3&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawLine);
		add_call("renderer2D_drawQuad(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawQuad);
		add_call("renderer2D_drawCircle(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&,single,single)", Graphics2D::drawCircle);
		add_call("renderer2D_drawLineRect(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawLineRect);
		add_call("renderer2D_drawLineCube(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Graphics2D::drawLineCube);

		add_call("gizmo_drawLine(Stulu.Vector3&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawLine);
		add_call("gizmo_drawRect(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawRect);
		add_call("gizmo_drawOutlineCube(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawOutlineCube);
		add_call("gizmo_drawCircle(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&,single,single)", Gizmo::drawCircle);
		add_call("gizmo_drawCube(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawCube);
		add_call("gizmo_drawSphere(Stulu.Vector3&,Stulu.Quaternion&,Stulu.Vector3&,Stulu.Vector4&)", Gizmo::drawSphere);

	}
}