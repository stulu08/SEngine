#include "st_pch.h"
#include "GameObject.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {
	GameObject GameObject::GetById(entt::entity id, Registry* registry) {
		if (id == entt::null || registry == nullptr)
			return GameObject::null;
		if(registry->IsObjectValid(id))
			return { id, registry };
		return GameObject::null;
	}
}
