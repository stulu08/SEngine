#include "st_pch.h"
#include "Registry.h"

#include "GameObject.h"
#include "Components/Components.h"

namespace Stulu {
	GameObject Registry::Create(entt::entity id) {
		return Create("GameObject", id);
	}

	GameObject Registry::Create(const std::string& name, entt::entity id) {
		GameObject go = CreateEmpty(id);
		auto& base = go.addComponent<GameObjectBaseComponent>(!name.empty() ? name : "GameObject");
		go.addComponent<TransformComponent>();
		return go;
	}

	GameObject Registry::CreateEmpty(entt::entity id) {
		GameObject go;
		if (id == entt::null) {
			go = { m_registry.create(), this };
		}
		else {
			go = { m_registry.create((entt::entity)id), this };
			if (go.GetID() != id) {
				CORE_WARN("Warining Internal GameObject could not be assigned to ID: {0}", id);
				CORE_WARN("Instead it was given the ID: {0}", go.GetID());
			}
		}
		return go;
	}


	GameObject Registry::FindByName(const std::string& name) {
		auto view = m_registry.view<GameObjectBaseComponent>();
		for (auto gameObject : view)
		{
			if (view.get<GameObjectBaseComponent>(gameObject).name == name)
				return GameObject(gameObject, this);
		}
		return GameObject::null;
	}

	void Registry::Destroy(GameObject gameObject) {
		//destroy all childs
		TransformComponent& transform = gameObject.getComponent<TransformComponent>();
		for (entt::entity child : transform.GetChildren()) {
			this->Destroy({ child, this });
		}

		m_registry.destroy(gameObject.GetID());
	}
}

