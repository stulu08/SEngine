#include "st_pch.h"
#include "GameObject.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {
	bool GameObject::isValid() const {
		if (m_entity == entt::null || m_scene == nullptr)
			return false;
		return hasComponent<GameObjectBaseComponent>();
	}
	entt::entity GameObject::getId() const {
		return m_entity;
	}
	GameObject GameObject::getById(entt::entity id, Scene* scene) {
		if (id == entt::null || scene == nullptr)
			return GameObject::null;
		return { id, scene };
	}
}
