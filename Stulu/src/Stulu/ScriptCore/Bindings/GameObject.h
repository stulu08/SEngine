#pragma once
#include "Stulu/Scene/GameObject.h"

namespace StuluBindings {
	class GameObject {
	public:
		static inline void addComponent(uint32_t entity, void* type) {
			Stulu::GameObject g( entt::entity(entity), Stulu::Scene::activeScene() );
		}
	};
}