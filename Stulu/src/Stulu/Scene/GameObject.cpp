#include "st_pch.h"
#include "GameObject.h"

namespace Stulu {
	GameObject GameObject::null = { entt::entity{ entt::null }, nullptr};
	GameObject::GameObject(entt::entity entity, Scene* scene)
		: m_entity(entity), m_scene(scene){
	}
}