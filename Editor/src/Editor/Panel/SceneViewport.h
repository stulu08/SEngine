#pragma once
#include <Stulu.h>
#include <imgui/imgui.h>

namespace Stulu {
	class SceneViewportPanel {
	public:
		SceneViewportPanel() {};
		void draw(SceneCamera& cam, bool* open);

		uint32_t width = 1;
		uint32_t height = 1;

		glm::vec2 windowPos = glm::vec2(1.0f);
		float windowWidth = 1.0f;
		float windowHeight = 1.0f;

		bool focused = false;
		bool hovered = false;
	};
}