#pragma once
#include "Component.h"
#include "Stulu/Scripting/Managed/AssemblyManager.h"

namespace Stulu {

	class ScriptingComponent : public Component {
	public:
		ScriptingComponent() = default;
		ScriptingComponent(const ScriptingComponent& other);

		std::vector<Ref<MonoObjectInstance>> runtimeScripts;

		Mono::Array FetchObjectArray();

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::Node& node) override;
	};

}