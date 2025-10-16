#pragma once
#include "Component.h"
#include "Stulu/Scripting/Managed/AssemblyManager.h"
#include "Stulu/Scripting/Managed/MonoObjectInstance.h"
#include "Stulu/Scripting/Managed/Mono.h"
namespace Stulu {

	class STULU_API ScriptingComponent : public Component {
	public:
		ScriptingComponent() = default;
		ScriptingComponent(const ScriptingComponent& other);

		std::vector<Ref<MonoObjectInstance>> runtimeScripts;

		Mono::Array FetchObjectArray();

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::Node& node) override;

		// will work for all components which are, registered natively or derive from
		void AddComponent(Stulu::Mono::Type type);
		bool RemoveComponent(Stulu::Mono::Type type);
		bool HasComponent(Stulu::Mono::Type type);
		void* GetComponent(Stulu::Mono::Type type);


		void ManagedAddComponent(Mono::Class componentChildClass);
		bool ManagedHasComponent(Mono::Class componentChildClass) const;
		bool ManagedRemoveComponent(Mono::Class componentChildClass);
		Mono::Object ManagedGetComponent(Mono::Class componentChildClass) const;
	};

}