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
		auto view = GetAllWith<GameObjectBaseComponent>();
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

	static void AddToRegistry(ModelNode& node, Registry* registry, GameObject parentObject) {
		GameObject object = registry->Create(node.name);
		TransformComponent& transform = object.getComponent<TransformComponent>();
		
		if (parentObject.IsValid()) {
			transform.SetParent(parentObject);
		}
		transform.SetWorldPosition(node.Position);
		transform.SetWorldRotation(node.Rotation);
		transform.SetWorldScale(node.Scale);

		auto& meshNode = node.mesh;
		if (meshNode.mesh.IsValid()) {
			object.addComponent<MeshFilterComponent>().SetMesh(meshNode.mesh);
			MeshRendererComponent& renderer = object.addComponent<MeshRendererComponent>();
			if (meshNode.materials.size() > 0) {
				renderer.Materials = meshNode.materials;
			}
		}

		for (auto& childNode : node.children) {
			AddToRegistry(childNode, registry, object);
		}

	}

	void Registry::SpawnModelAsset(const ModelAsset& model) {
		ModelNode& rootNode = model->GetRootNode();
		AddToRegistry(rootNode, this, GameObject::null);
	}
}

