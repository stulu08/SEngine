#include "st_pch.h"
#include "GameObject.h"
#include "Stulu/Scene/Components.h"

namespace Stulu {
	GameObject GameObject::null = { entt::entity{ entt::null }, nullptr};
	GameObject::GameObject(entt::entity entity, Scene* scene)
		: m_entity(entity), m_scene(scene){
	}
	UUID GameObject::getId() {
		return getComponent<GameObjectBaseComponent>().uuid;
	}
	GameObject GameObject::getById(UUID& id, Scene* scene) {
		GameObject object = GameObject::null;
		scene->m_registry.view<GameObjectBaseComponent>().each([&](entt::entity ent, GameObjectBaseComponent& comp) {
			if (!object && comp.uuid == id) {
				object = {ent, scene };
			}
		});
		return object;
	}
}
