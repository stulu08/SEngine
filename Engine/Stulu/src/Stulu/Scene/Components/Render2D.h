#pragma once
#include "Component.h"
#include "Stulu/Renderer/Renderer2D.h"
#include "Stulu/Resources/Assets/TextureAssets.h"

namespace Stulu {
	class SpriteRendererComponent : public Component {
	public:
		glm::vec4 color = COLOR_WHITE;
		glm::vec2 tiling = glm::vec2(1.0f);

		Texture2DAsset texture;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: color(color) {};

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "color" << YAML::Value << color;
			out << YAML::Key << "tiling" << YAML::Value << tiling;
			if(texture.IsValid())
				out << YAML::Key << "texture" << YAML::Value << texture.GetUUID();
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["color"])
				color = node["color"].as<glm::vec4>();
			if (node["tiling"])
				tiling = node["tiling"].as<glm::vec2>();
			if (node["texture"])
				texture = AssetsManager::GlobalInstance().GetAsset<Texture2DAsset>(node["texture"].as<UUID>());
		}
	};
	class CircleRendererComponent : public Component {
	public:
		glm::vec4 color = COLOR_WHITE;
		float thickness = 1.0f;
		float fade = .005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
		CircleRendererComponent(const glm::vec4 color)
			: color(color) {};

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "color" << YAML::Value << color;
			out << YAML::Key << "thickness" << YAML::Value << thickness;
			out << YAML::Key << "fade" << YAML::Value << fade;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["color"])
				color = node["color"].as<glm::vec4>();
			if (node["thickness"])
				thickness = node["thickness"].as<float>();
			if (node["fade"])
				fade = node["fade"].as<float>();
		}
	};
}