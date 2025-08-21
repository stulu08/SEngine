#include "st_pch.h"
#include "Render3D.h"

namespace Stulu {
	void LightComponent::Serialize(YAML::Emitter& out) const {
		out << YAML::Key << "Type" << YAML::Value << (uint32_t)Type;
		out << YAML::Key << "Color" << YAML::Value << Color;
		out << YAML::Key << "Strength" << YAML::Value << Strength;
		out << YAML::Key << "CastShadows" << YAML::Value << CastShadows;

		out << YAML::Key << "Shadows" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Strength" << YAML::Value << Shadows.Strength;
		out << YAML::Key << "Bias" << YAML::Value << Shadows.Bias;
		out << YAML::Key << "TextureSize" << YAML::Value << Shadows.TextureSize;
		out << YAML::Key << "ShadowMode" << YAML::Value << (uint32_t)Shadows.ShadowMode;
		out << YAML::Key << "Soft" << YAML::Value << Shadows.Soft;
		out << YAML::EndMap;

		out << YAML::Key << "Directional" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Size" << YAML::Value << Directional.Size;
		out << YAML::Key << "IsMainLight" << YAML::Value << Directional.IsMainLight;
		out << YAML::EndMap;

		out << YAML::Key << "Point" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Radius" << YAML::Value << Point.Radius;
		out << YAML::EndMap;

		out << YAML::Key << "Spot" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "InnerCutoff" << YAML::Value << Spot.InnerCutoff;
		out << YAML::Key << "OuterCutoff" << YAML::Value << Spot.OuterCutoff;
		out << YAML::EndMap;
	}
	void LightComponent::Deserialize(YAML::Node& node) {
		if (node["Type"])
			Type = (LightType)node["Type"].as<uint32_t>();
		if (node["Color"])
			Color = node["Color"].as<glm::vec3>();
		if (node["Strength"])
			Strength = node["Strength"].as<float>();
		if (node["CastShadows"])
			CastShadows = node["CastShadows"].as<bool>();

		if (node["Shadows"]) {
			auto ShadowsNode = node["Shadows"];
			Shadows.Strength = ShadowsNode["Strength"].as<float>(Shadows.Strength);
			Shadows.Bias = ShadowsNode["Bias"].as<float>(Shadows.Bias);
			Shadows.TextureSize = ShadowsNode["TextureSize"].as<uint32_t>(Shadows.TextureSize);
			Shadows.ShadowMode = (LightShadowMode)ShadowsNode["ShadowMode"].as<uint32_t>((uint32_t)Shadows.ShadowMode);
			Shadows.Soft = ShadowsNode["Soft"].as<bool>(Shadows.Soft);
		}
		if (node["Directional"]) {
			auto DirectionalNode = node["Directional"];
			Directional.Size = DirectionalNode["Size"].as<float>(Directional.Size);
			Directional.IsMainLight = DirectionalNode["IsMainLight"].as<bool>(Directional.IsMainLight);
		}
		if (node["Point"]) {
			auto PointNode = node["Point"];
			Point.Radius = PointNode["Radius"].as<float>(Point.Radius);
		}
		if (node["Spot"]) {
			auto SpotNode = node["Spot"];
			Spot.InnerCutoff = SpotNode["InnerCutoff"].as<float>(Spot.InnerCutoff);
			Spot.OuterCutoff = SpotNode["OuterCutoff"].as<float>(Spot.OuterCutoff);
		}
	}
}
