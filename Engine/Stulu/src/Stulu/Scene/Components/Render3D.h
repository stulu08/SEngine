#pragma once
#include "Component.h"

#include "Stulu/Scene/AssetsManager.h"
#include "Stulu/Scene/Material.h"

namespace Stulu {
	class LightComponent : public Component {
	public:
		enum Type { Directional = 0, Area = 1, Spot = 2 };

		Type lightType = Type::Directional;
		glm::vec3 color = glm::vec3(1.0f);
		float strength = 1.0f;

		float areaRadius = 1.0f;

		float spotLight_cutOff = 10.0f;
		float spotLight_outerCutOff = 15.0f;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const Type& type)
			: lightType(type) {};
	};


	class SkyBoxComponent : public Component {
	public:
		Ref<SkyBox> texture = nullptr;
		glm::vec3 rotation = { 0,0,0 };

		SkyBoxComponent() = default;
		SkyBoxComponent(const SkyBoxComponent&) = default;
	};

	class MeshRendererComponent : public Component {
	public:
		Ref<Material> material = nullptr;
		CullMode cullmode = CullMode::Back;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;

		virtual void onComponentAdded(Scene* scene) override;
	};
	class MeshFilterComponent : public Component {
	public:
		MeshAsset mesh{ "",nullptr };

		MeshFilterComponent() = default;
		MeshFilterComponent(const MeshFilterComponent&) = default;
		MeshFilterComponent(const Ref<Mesh>& _mesh) { setMesh(_mesh); }
		MeshFilterComponent(MeshAsset& mesh)
			: mesh(mesh) {}

		void setMesh(Ref<Mesh> meshObj, const std::string& name = "New Mesh") {
			if (meshObj)
				mesh.hasMesh = true;
			mesh.mesh = meshObj;
			mesh.name = name;
		}
		void setMesh(MeshAsset meshAsset) {
			mesh = meshAsset;
		}

		virtual void onComponentAdded(Scene* scene) override;
	};


	inline void MeshRendererComponent::onComponentAdded(Scene* scene) {
		gameObject.saveAddComponent<MeshFilterComponent>();
	}
	inline void MeshFilterComponent::onComponentAdded(Scene* scene) {
		gameObject.saveAddComponent<MeshRendererComponent>();
	}
}