#include "st_pch.h"
#include "GameObject.h"
#include "Stulu/Scene/Components/Components.h"

namespace Stulu {
	GameObject GameObject::null = { entt::entity{ entt::null }, nullptr};
	GameObject::GameObject(entt::entity entity, Scene* scene)
		: m_entity(entity), m_scene(scene){
	}
	bool GameObject::isValid() const {
		if (m_entity == entt::null || m_scene == nullptr)
			return false;
		if (hasComponent<GameObjectBaseComponent>() && hasComponent<TransformComponent>())
			if(getId() != UUID::null)
				return true;
		return false;
	}
	UUID GameObject::getId() const {
		return getComponent<GameObjectBaseComponent>().uuid;
	}
	GameObject GameObject::getById(const UUID& id, Scene* scene) {
		ST_PROFILING_FUNCTION();
		GameObject object = GameObject::null;
		scene->m_registry.view<GameObjectBaseComponent>().each([&](entt::entity ent, GameObjectBaseComponent& comp) {
			if (comp.uuid == id) {
				object = GameObject(ent,scene);
				return;
			}
		});
		return object;
	}
}
