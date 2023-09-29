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
		if (!m_scene->m_registry.valid(m_entity))
			return false;
		if (hasComponent<GameObjectBaseComponent>() && hasComponent<TransformComponent>())
			return getComponent<GameObjectBaseComponent>().getUUID() != UUID::null;
		return false;
	}
	UUID GameObject::getId() const {
		if(hasComponent<GameObjectBaseComponent>())
			return getComponent<GameObjectBaseComponent>().getUUID();
		return UUID::null;
	}
	GameObject GameObject::getById(const UUID& id, Scene* scene) {
		ST_PROFILING_FUNCTION();
		if (id == UUID::null || scene == nullptr)
			return GameObject::null;
		auto pos = scene->m_uuidGameObjectMap.find(id);
		if (pos != scene->m_uuidGameObjectMap.end()) {
			return GameObject((*pos).second, scene);
		}
		return GameObject::null;
	}
}
