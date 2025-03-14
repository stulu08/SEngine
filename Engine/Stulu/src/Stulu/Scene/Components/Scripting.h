#pragma once
#include "Stulu/Scripting/Managed/AssemblyManager.h"
namespace Stulu {

	class ScriptingComponent : public Component {
	public:
		ScriptingComponent() = default;
		ScriptingComponent(const ScriptingComponent& other)
			: Component(other) {
			for (auto& script : other.runtimeScripts) {
				runtimeScripts.push_back(createRef<MonoObjectInstance>(*script));
			}
		}
		std::vector<Ref<MonoObjectInstance>> runtimeScripts;

		Mono::Array FetchObjectArray() {
			const auto& manager = Application::get().getAssemblyManager();

			Mono::Array list = Mono::Array::New(manager->getCoreDomain(), manager->getComponentClass(), runtimeScripts.size());
			for (size_t i = 0; i < runtimeScripts.size(); i++) {
				list.SetRef(i, runtimeScripts[i]->getObject());
			}
			return list;
		}
	};

}