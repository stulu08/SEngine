#include "st_pch.h"
#include "Scripting.h"

namespace Stulu {
	ScriptingComponent::ScriptingComponent(const ScriptingComponent& other)
		: Component(other) {
		for (auto& script : other.runtimeScripts) {
			runtimeScripts.push_back(createRef<MonoObjectInstance>(*script));
		}
	}

	Mono::Array ScriptingComponent::FetchObjectArray() {
		const auto& manager = Application::get().getAssemblyManager();

		Mono::Array list = Mono::Array::New(manager->getCoreDomain(), manager->getComponentClass(), runtimeScripts.size());
		for (size_t i = 0; i < runtimeScripts.size(); i++) {
			list.SetRef(i, runtimeScripts[i]->getObject());
		}
		return list;
	}
	void ScriptingComponent::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
		for (Ref<MonoObjectInstance> instance : runtimeScripts) {
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << instance->getClass().GetName();
			out << YAML::Key << "Namespace" << YAML::Value << instance->getClass().GetNamespace();

			out << YAML::Key << "Fields" << YAML::Value << YAML::BeginSeq;
			for (auto& field : instance->getFields()) {
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << field->getName();
				out << YAML::Key << "TypeName" << field->getTypeName();
				field->Serialize(out);
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
	}
	void ScriptingComponent::Deserialize(YAML::Node& node) {
		const auto& manager = Application::get().getAssemblyManager();
		for (auto inst : node["Scripts"]) {
			Mono::Class exists = Mono::Class::FromName(manager->getAppAssembly()->getImage(), inst["Namespace"].as<std::string>(), inst["Name"].as<std::string>());
			if (exists) {
				Ref<MonoObjectInstance> object = createRef<MonoObjectInstance>(exists, manager->getAppAssembly().get());
				for (YAML::detail::iterator_value field : inst["Fields"]) {
					if (!field["Name"] || !field["TypeName"])
						continue;

					std::string name = field["Name"].as<std::string>();
					std::string typeName = field["TypeName"].as<std::string>();

					for (const auto& fieldItem : object->getFields()) {
						if (fieldItem->getName() != name || fieldItem->getTypeName() != typeName)
							continue;
						fieldItem->Deserialize(field);
					}
				}
				runtimeScripts.push_back(object);
			}
		}
	}
	void ScriptingComponent::AddComponent(Stulu::Mono::Type type) {
		const auto& manager = Stulu::Application::get().getAssemblyManager();
		std::string typeName = type.GetNameFull(Stulu::Mono::TypeNameFormat::FULL_NAME);

		auto& componentRegister = manager->GetComponentRegister_Add();
		if (componentRegister.find(typeName) != componentRegister.end()) {
			return componentRegister[typeName](this->gameObject);
		}

		// c# scripts
		Stulu::Mono::Class desiredClass = type.GetClass();
		if (!desiredClass)
			return;

		Stulu::Mono::Class parent = desiredClass.GetParent();
		if (!parent)
			return;

		if (parent == manager->getComponentClass()) {
			ManagedAddComponent(desiredClass);
		}
	}
	bool ScriptingComponent::RemoveComponent(Stulu::Mono::Type type) {
		const auto& manager = Stulu::Application::get().getAssemblyManager();
		std::string typeName = type.GetNameFull(Stulu::Mono::TypeNameFormat::FULL_NAME);

		auto& componentRegister = manager->GetComponentRegister_Remove();
		if (componentRegister.find(typeName) != componentRegister.end()) {
			return componentRegister[typeName](gameObject);
		}

		// c# scripts
		Stulu::Mono::Class desiredClass = type.GetClass();
		if (!desiredClass)
			return false;

		Stulu::Mono::Class parent = desiredClass.GetParent();
		if (!parent)
			return false;

		if (parent == manager->getComponentClass()) {
			return ManagedRemoveComponent(desiredClass);
		}

		return false;
	}
	bool ScriptingComponent::HasComponent(Stulu::Mono::Type type) {
		const auto& manager = Stulu::Application::get().getAssemblyManager();
		std::string typeName = type.GetNameFull(Stulu::Mono::TypeNameFormat::FULL_NAME);

		auto& componentRegister = manager->GetComponentRegister_Has();
		if (componentRegister.find(typeName) != componentRegister.end()) {
			return componentRegister[typeName](gameObject);
		}

		// c# scripts
		Stulu::Mono::Class desiredClass = type.GetClass();
		if (!desiredClass)
			return false;

		Stulu::Mono::Class parent = desiredClass.GetParent();
		if (!parent)
			return false;

		if (parent == manager->getComponentClass()) {
			return ManagedHasComponent(desiredClass);
		}
		return false;
	}
	void* ScriptingComponent::GetComponent(Stulu::Mono::Type type) {
		if (gameObject != Stulu::GameObject::null) {
			Stulu::Mono::Class desired = type.GetClass();
			Stulu::Mono::Object object = ManagedGetComponent(desired);
			if (object)
				return (void*)object;
		}
		return nullptr;
	}


	void ScriptingComponent::ManagedAddComponent(Mono::Class componentChildClass) {
		auto appAssembly = Application::get().getAssemblyManager()->getAppAssembly().get();
		auto object = createRef<MonoObjectInstance>(componentChildClass, appAssembly);
		runtimeScripts.push_back(object);
	}
	bool ScriptingComponent::ManagedHasComponent(Mono::Class componentChildClass) const {
		for (auto& script : runtimeScripts) {
			if (script->getClass() == componentChildClass) {
				return true;
			}
		}
		return false;
	}
	bool ScriptingComponent::ManagedRemoveComponent(Mono::Class componentChildClass) {
		for (uint32_t i = 0; i < runtimeScripts.size(); i++) {
			if (runtimeScripts[i]->getClass() == componentChildClass) {
				runtimeScripts.erase(runtimeScripts.begin() + i);
				return true;
			}
		}
		return false;
	}
	Mono::Object ScriptingComponent::ManagedGetComponent(Mono::Class componentChildClass) const {
		for (auto& runtimeScripts : runtimeScripts) {
			if (runtimeScripts->getClass() == componentChildClass) {
				return runtimeScripts->getObject();
			}
		}
		CORE_ERROR("GameObject does not have component");
		return nullptr;
	}
}