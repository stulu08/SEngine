#include "st_pch.h"
#include "GameObject.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {
	GameObject GameObject::GetById(entt::entity id, Scene* scene) {
		if (id == entt::null || scene == nullptr)
			return GameObject::null;
		if(scene->getRegistry().valid(id))
			return { id, scene };

		return GameObject::null;
	}
}
