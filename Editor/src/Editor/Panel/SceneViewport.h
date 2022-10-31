#pragma once
#include <Stulu.h>
#include <imgui/imgui.h>

namespace Stulu {
	class SceneViewportPanel {
	public:
		SceneViewportPanel() {};
		void draw(SceneCamera& cam, bool* open);

		ImVec2 startPos;
		uint32_t width = 1;
		uint32_t height = 1;

		bool focused = false;
		bool hovered = false;
		bool drawn = false;
	};
}