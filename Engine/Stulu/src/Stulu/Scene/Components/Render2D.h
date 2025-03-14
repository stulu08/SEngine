#pragma once
#include "Component.h"
#include "Stulu/Renderer/Renderer2D.h"

namespace Stulu {
	class SpriteRendererComponent : public Component {
	public:
		glm::vec4 color = COLOR_WHITE;
		glm::vec2 tiling = glm::vec2(1.0f);

		Ref<Texture> texture = nullptr;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4 color)
			: color(color) {};
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
	};
}