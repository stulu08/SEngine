#pragma once
#include "Component.h"

#include "Transform.h"
#include <Stulu/Resources/Assets/MaterialAsset.h>

namespace Stulu {
	enum class LightType { 
		Directional = 0, 
		Point = 1, 
		Spot = 2 
	};
	enum class LightShadowMode {
		// Updates every frame
		Realtime = 0,
		// Shadows will be generated at scene start up
		Static
	};

	class STULU_API LightComponent : public Component {
	public:
		LightType Type = LightType::Directional;
		glm::vec3 Color = glm::vec3(1.0f);
		float Strength = 1.0f;

		bool CastShadows = false;
		// Will be ignored for the main light
		struct ShadowData {
			LightShadowMode ShadowMode = LightShadowMode::Realtime;
			uint32_t TextureSize = 256;
			float Bias = 0.0005f;
			float Strength = 1.0f;
			bool Soft = true;
		}Shadows;


		struct DirectionalData {
			float Size = 1.0f;
			// Will be used as the main scene light (Cascaded Sun)
			bool IsMainLight = true;
		}Directional;

		struct PointData {
			float Radius = 5.0f;
		}Point;

		struct SpotData {
			float InnerCutoff = 5.0f;
			float OuterCutoff = 10.0f;
		}Spot;

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		LightComponent(const LightType& type)
			: Type(type) {};

		virtual void Serialize(YAML::Emitter& out) const override;
		virtual void Deserialize(YAML::Node& node) override;

		inline LightBufferData::Light GetRenderData() const {
			LightBufferData::Light data;
			const TransformComponent& transform = gameObject.getComponent<TransformComponent>();
			data.ColorStrength = glm::vec4(Color, Strength);
			data.PositionType = glm::vec4(transform.GetWorldPosition(), (float)Type);
			data.Direction = glm::vec4(transform.GetForward(), 1.0f);
			data.ShadowData = glm::vec4(-1.0f, Shadows.Bias, Shadows.Strength, Shadows.Soft);
			switch (Type)
			{
			case Stulu::LightType::Directional:
				data.Data = glm::vec4(Directional.Size, (float)Directional.IsMainLight, 1.0f, 1.0f);
				break;
			case Stulu::LightType::Point:
				data.Data = glm::vec4(Point.Radius, 1.0f, 1.0f, 1.0f);
				break;
			case Stulu::LightType::Spot:
				data.Data = glm::vec4(Spot.InnerCutoff, Spot.OuterCutoff, 1.0f, 1.0f);
				break;
			}

			return data;
		}
	};

	class SkyBoxComponent : public Component {
	public:
		MaterialAsset material;
		glm::vec3 rotation = { 0,0,0 };
		float LightStrength = 1.0f;
		float ReflectionIntensity = 1.0f;

		SkyBoxComponent() = default;
		SkyBoxComponent(const SkyBoxComponent&) = default;

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "rotation" << YAML::Value << rotation;
			out << YAML::Key << "LightStrength" << YAML::Value << LightStrength;
			out << YAML::Key << "ReflectionIntensity" << YAML::Value << ReflectionIntensity;
			if (material.IsValid())
				out << YAML::Key << "SkyBox" << YAML::Value << material.GetUUID();
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["rotation"])
				rotation = node["rotation"].as<glm::vec3>();
			if (node["LightStrength"])
				LightStrength = node["LightStrength"].as<float>();
			if (node["ReflectionIntensity"])
				ReflectionIntensity = node["ReflectionIntensity"].as<float>();

			if (node["SkyBox"])
				material = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(node["SkyBox"].as<UUID>());
			if (node["texture"]) {
				UUID texNode = node["texture"].as<UUID>();
				if (AssetsManager::GlobalInstance().TypeCheck<MaterialAsset>(texNode)) {
					material = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(texNode);
				}
			}
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
		const MeshAsset& GetMesh() const {
			return mesh;
		}
		MeshAsset& GetMesh() {
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

	// internal usage
	class STULU_API RendererComponent : public Component {
	public:
		// Value will be written to the stencil buffer in the next frame
		uint8_t StencilValue = 0x00;
		CullMode cullmode = CullMode::Back;
		bool CastShadows = true;
	};

	class STULU_API MeshRendererComponent : public RendererComponent {
	public:
		std::vector<MaterialAsset> Materials;

		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent&) = default;

		inline size_t GetMaterialCount() const { return Materials.size(); }
		inline void AddMaterial(const MaterialAsset& asset) {
			Materials.push_back(asset);
		}
		inline void SetMaterial(const MaterialAsset& asset, uint32_t index = 0) {
			if (index < Materials.size()) {
				Materials[index] = asset;
			}
			else {
				Materials.insert(Materials.begin() + index, asset);
			}
		}
		inline MaterialAsset GetMaterial(size_t index = 0) const {
			if (index < Materials.size())
				return Materials.at(glm::min(index, GetMaterialCount() - 1));
			else
				return AssetsManager::InvalidAsset<MaterialAsset>();
		}

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "cullmode" << YAML::Value << (int)cullmode;
			out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;

			std::vector<UUID> materialIDs;
			for (const auto& material : Materials) {
				materialIDs.push_back(material.GetUUID());
			}
			out << YAML::Key << "Materials" << YAML::Value << materialIDs;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["cullmode"])
				cullmode = (CullMode)node["cullmode"].as<int>();
			if (node["CastShadows"])
				CastShadows = node["CastShadows"].as<bool>();

			if (node["material"]) {
				MaterialAsset mat = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(node["material"].as<UUID>());
				SetMaterial(mat);
			}
			if (node["Materials"]) {
				std::vector<UUID> materialIDs = node["Materials"].as<std::vector<UUID>>();
				for (const auto& id : materialIDs) {
					AddMaterial(AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(id));
				}
			}
		}
	};

	class STULU_API SkinnedMeshRenderer : public RendererComponent {
	public:
		MaterialAsset Material;

		inline MaterialAsset GetMaterial() const {
			return Material;
		}

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "cullmode" << YAML::Value << (int)cullmode;
			out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;
			out << YAML::Key << "Material" << YAML::Value << Material.GetUUID();
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["cullmode"])
				cullmode = (CullMode)node["cullmode"].as<int>();
			if (node["CastShadows"])
				CastShadows = node["CastShadows"].as<bool>();
			if (node["material"])
				Material = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(node["material"].as<UUID>());
		}
	};
}