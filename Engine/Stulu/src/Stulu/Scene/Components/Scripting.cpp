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
				out << YAML::Key << "Type" << (uint32_t)field->getType();
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
					std::string name = field["Name"].as<std::string>();
					ManagedPropertyType type = (ManagedPropertyType)field["Type"].as<uint32_t>();

					for (auto fieldItem : object->getFields()) {
						if (fieldItem->getName() != name || fieldItem->getType() != type)
							continue;
						fieldItem->Deserialize(field);

					}
				}
				runtimeScripts.push_back(object);
			}
		}
	}
}