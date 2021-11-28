#pragma once
#include <Stulu.h>

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

namespace Stulu {
	class SceneViewportPanel {
	public:
		SceneViewportPanel() {};
		void draw(SceneCamera& cam, GameObject selected, bool* open);

		uint32_t width = 1;
		uint32_t height = 1;
		bool focused = false;
		bool hovered = false;
		int gizmoEditType = -1;
	};
}