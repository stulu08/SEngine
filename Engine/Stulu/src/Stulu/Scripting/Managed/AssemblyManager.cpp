#include "st_pch.h"
#include "AssemblyManager.h"

#include "Bindings/Bindings.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Resources/Resources.h"
#include "Stulu/Serialization/YAML.h"
#include "Stulu/Scene/Components/Components.h"
#include "Stulu/Scene/GameObject.h"

namespace Stulu {
	AssemblyManager::AssemblyManager(const std::string& assemblyPath, const std::string& coreAssemblyPath, bool debuggin,
		const std::string& monoAssemblyPath, const std::string& monoConfigPath) 
		: m_corePath(coreAssemblyPath), m_appPath(assemblyPath), m_enabledDebugging(debuggin) {
		
		InitMono();
		CreateAppDomain();
		LoadAssemblies();
		LoadDefaults();
	}

	AssemblyManager::~AssemblyManager() {
		m_monoDomain.Set(true);
		m_coreDomain.Unload();

		Mono::JIT::Cleanup(m_monoDomain);
	}

	void AssemblyManager::InitMono() {
		Mono::SetDirs(Resources::EngineDataDir + "/mono/lib/", Resources::EngineDataDir + "/mono/etc/");

		if (m_enabledDebugging) {
			static std::array<const char*, 2> argv = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=MonoDebugger.log",
				"--soft-breakpoints"
			};
			Mono::JIT::ParseOptions((int)argv.size(), (char**)argv.data());
			Mono::Debug::Init(Mono::Debug::Format::MONO);
		}

		m_monoDomain = Mono::JIT::Init("StuluEngine");
		if (!m_monoDomain) {
			CORE_ERROR("Mono Domain creation failed");
			return;
		}

		if (m_enabledDebugging) {
			Mono::Debug::CreateDomain(m_monoDomain);
		}

		Mono::Thread::SetMain(Mono::Thread::GetCurrent());
	}
	
	void AssemblyManager::CreateAppDomain() {
		m_coreDomain = Mono::Domain::CreateAppDomain("StuluRuntimeCore");
		m_coreDomain.Set(true);
	}
	
	void AssemblyManager::LoadAssemblies() {
		m_scriptCoreAssembly = createRef<ScriptAssembly>(m_corePath.string(), m_enabledDebugging);
		m_appAssembly = createRef<ScriptAssembly>(m_appPath.string(), m_enabledDebugging);
	}

	void AssemblyManager::LoadDefaults() {
		RegisterProperty<Int32Property>("System.Int32");
		RegisterProperty<UInt32Property>("System.UInt32");
		RegisterProperty<UInt64Property>("System.UInt64");
		RegisterProperty<FloatProperty>("System.Single");
		RegisterProperty<BoolProperty>("System.Boolean");
		RegisterProperty<Vector2Property>("Stulu.Vector2");
		RegisterProperty<Vector3Property>("Stulu.Vector3");
		RegisterProperty<Vector4Property>("Stulu.Vector4");
		RegisterProperty<AssetProperty>("Stulu.Texture2D");
		RegisterProperty<AssetProperty>("Stulu.Material");
		RegisterProperty<AssetProperty>("Stulu.Mesh");
		RegisterProperty<GameObjectProperty>("Stulu.GameObject");

		m_gameObjectAttachedClass = m_scriptCoreAssembly->CreateClass("Stulu", "GameObjectAttached");
		m_componentClass = m_scriptCoreAssembly->CreateClass("Stulu", "Component");
		m_exceptionStackTraceMethod = Mono::GetExceptionClass().GetMethodFromName("get_StackTrace", 0);
		
		m_events.onAwake = m_componentClass.GetMethodFromName("Impl_onAwake", 0);
		m_events.onStart = m_componentClass.GetMethodFromName("Impl_onStart", 0);
		m_events.onUpdate = m_componentClass.GetMethodFromName("Impl_onUpdate", 0);
		m_events.onRender = m_componentClass.GetMethodFromName("Impl_onRender", 0);
		m_events.onRender2D = m_componentClass.GetMethodFromName("Impl_onRender2D", 0);
		m_events.onDrawGizmos = m_componentClass.GetMethodFromName("Impl_onDrawGizmos", 0);
		m_events.onDestroy = m_componentClass.GetMethodFromName("Impl_onDestroy", 0);
		m_events.onSceneExit = m_componentClass.GetMethodFromName("Impl_onSceneExit", 0);

		StuluBindings::LoadCoreBindings();
		StuluBindings::LoadComponentsBindings();

		LoadComponents();
	}

	void AssemblyManager::Reload() {
		m_appAssembly->Unload();
		//m_scriptCoreAssembly->Unload();

		//CreateAppDomain();
		//m_scriptCoreAssembly->Load(m_corePath.string());
		m_appAssembly->Load(m_appPath.string(), m_enabledDebugging);

		LoadDefaults();
	}

	void AssemblyManager::LoadComponents() {
		m_components.clear();
		m_components = m_appAssembly->LoadAllClasses(m_componentClass);
	}

	void AssemblyManager::ManagedAddComponent(GameObject gameObject, Mono::Class componentChildClass) const {
		Stulu::ScriptingComponent& comp = gameObject.saveAddComponent<Stulu::ScriptingComponent>();
		auto object = Stulu::createRef<Stulu::MonoObjectInstance>(componentChildClass, m_appAssembly.get());
		comp.runtimeScripts.push_back(object);
	}
	bool AssemblyManager::ManagedHasComponent(GameObject gameObject, Mono::Class componentChildClass) const {
		auto& comp = gameObject.saveAddComponent<Stulu::ScriptingComponent>();
		for (auto& script : comp.runtimeScripts) {
			if (script->getClass() == componentChildClass) {
				return true;
			}
		}
		return false;
	}
	bool AssemblyManager::ManagedRemoveComponent(GameObject gameObject, Mono::Class componentChildClass) const {
		auto& scripts = gameObject.saveAddComponent<Stulu::ScriptingComponent>().runtimeScripts;
		for (uint32_t i = 0; i < scripts.size(); i++) {
			if (scripts[i]->getClass() == componentChildClass) {
				scripts.erase(scripts.begin() + i);
				return true;
			}
		}
		return false;
	}
	Mono::Object AssemblyManager::ManagedGetComponent(GameObject gameObject, Mono::Class componentChildClass) const {
		auto& scripts = gameObject.saveAddComponent<Stulu::ScriptingComponent>().runtimeScripts;
		for (auto& script : scripts) {
			if (script->getClass() == componentChildClass) {
				return script->getObject();
			}
		}
		CORE_ERROR("GameObject does not have component");
		return nullptr;
	}
}
