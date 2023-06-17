#pragma once
#include <Stulu.h>
#include "imgui/imgui.h"
namespace Stulu {
	class GameViewportPanel {
	public:
		GameViewportPanel() {
			ImVec2 viewportSize = sizes[m_selectedSize];
			width = (uint32_t)viewportSize.x;
			height = (uint32_t)viewportSize.y;
			windowPos = glm::vec2(.0f);
		}
		void draw(const Ref<FrameBuffer>& frameBuffer, bool* open);

		uint32_t width = 1;
		uint32_t height = 1;
		bool focused = false;
		bool hovered = false;
		bool drawn = false;
	private:
		void drawErrorImage();
		int m_selectedSize = 1;
		float zoom = 0.5f;
		glm::vec2 windowPos;
		ImVec2 sizes[6] = {
			ImVec2{ 1920,1080 }, ImVec2{ 1000,1000 },
			ImVec2{ 1280,720 }, ImVec2{ 500,500 },
			ImVec2{ 250,250 }, ImVec2{ 1280,720 }
		};

		friend class EditorLayer;
	};
}

