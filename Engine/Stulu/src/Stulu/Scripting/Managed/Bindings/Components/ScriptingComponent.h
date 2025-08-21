#pragma once

namespace StuluBindings {
	class ScriptingComponent {
	public:
		static inline MonoArray* FetchObjectArray(uint64_t go) {
			auto gameObject = Stulu::GameObject((entt::entity)go, GetCurrentRegistry());
			if(gameObject.hasComponent<Stulu::ScriptingComponent>())
				return gameObject.getComponent<Stulu::ScriptingComponent>().FetchObjectArray();
			return Stulu::Mono::Array::New(getCoreDomain(), Stulu::Mono::GetObjectClass(), 0);
		}
	};
}