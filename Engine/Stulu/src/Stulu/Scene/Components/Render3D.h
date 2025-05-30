#pragma once
#include "Component.h"

#include "Transform.h"
#include <Stulu/Resources/Assets/MaterialAsset.h>

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

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "lightType" << YAML::Value << (int)lightType;
			out << YAML::Key << "color" << YAML::Value << color;
			out << YAML::Key << "strength" << YAML::Value << strength;
			out << YAML::Key << "areaRadius" << YAML::Value << areaRadius;
			out << YAML::Key << "spotLight_cutOff" << YAML::Value << spotLight_cutOff;
			out << YAML::Key << "spotLight_outerCutOff" << YAML::Value << spotLight_outerCutOff;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["lightType"])
				lightType = (Type)node["lightType"].as<int>();
			if (node["color"])
				color = node["color"].as<glm::vec3>();
			if (node["strength"])
				strength = node["strength"].as<float>();
			if (node["areaRadius"])
				areaRadius = node["areaRadius"].as<float>();
			if (node["spotLight_cutOff"])
				spotLight_cutOff = node["spotLight_cutOff"].as<float>();
			if (node["spotLight_outerCutOff"])
				spotLight_outerCutOff = node["spotLight_outerCutOff"].as<float>();
		}
	};


	class SkyBoxComponent : public Component {
	public:
		SkyBoxAsset texture;
		glm::vec3 rotation = { 0,0,0 };

		SkyBoxComponent() = default;
		SkyBoxComponent(const SkyBoxComponent&) = default;

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "rotation" << YAML::Value << rotation;
			if (texture.IsValid())
				out << YAML::Key << "texture" << YAML::Value << texture.GetUUID();
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["rotation"])
				rotation = node["rotation"].as<glm::vec3>();
			if (node["texture"])
				texture = AssetsManager::GlobalInstance().GetAsset<SkyBoxAsset>(node["texture"].as<UUID>());
		}
	};

	class MeshRendererComponent : public Component {
	public:
		MaterialAsset material;
		CullMode cullmode = CullMode::Back;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "cullmode" << YAML::Value << (int)cullmode;
			if (material.IsValid())
				out << YAML::Key << "material" << YAML::Value << material.GetUUID();
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["cullmode"])
				cullmode = (CullMode)node["cullmode"].as<int>();
			if (node["material"])
				material = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(node["material"].as<UUID>());
		}
	};
	class MeshFilterComponent : public Component {
	public:

		MeshFilterComponent() = default;
		MeshFilterComponent(const MeshFilterComponent&) = default;
		MeshFilterComponent(MeshAsset& meshAsset) {
			SetMesh(meshAsset);
		}

		void SetMesh(const MeshAsset& meshAsset) {
			this->mesh = meshAsset;
			if (mesh.IsLoaded()) {
				TransformComponent& transform = gameObject.saveAddComponent<TransformComponent>();
				transform.SetBounds(mesh->GetBoundingBox());
			}
		}
		MeshAsset GetMesh() const {
			return mesh;
		}


		virtual void Serialize(YAML::Emitter& out) const override {
			if (mesh.IsValid())
				out << YAML::Key << "mesh" << YAML::Value << mesh.GetUUID();
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["mesh"]) {
				auto asset = AssetsManager::GlobalInstance().GetAsset<MeshAsset>(node["mesh"].as<UUID>());
				SetMesh(asset);
			}
		}

	private:
		MeshAsset mesh;
	};
}