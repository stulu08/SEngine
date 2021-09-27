#include "EditorViewport.h"
#include <imgui/imgui.h>

namespace Stulu {
	EditorViewportPanel::EditorViewportPanel(const char* name) {
		m_name = name;
	}
	void EditorViewportPanel::draw(Ref<Texture> texture, bool end) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		if (ImGui::Begin(m_name)) {

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			m_viewPortPanelWidth = (uint32_t)viewportSize.x;
			m_viewPortPanelHeight = (uint32_t)viewportSize.y;

			ImTextureID viewPortTexture = (void*)texture->getRendererID();
			ImGui::Image(viewPortTexture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

			m_viewPortFocused = ImGui::IsWindowFocused();
			m_viewPortHovered = ImGui::IsWindowHovered();
		}
		if (end)
			endDraw();
	}
	void EditorViewportPanel::endDraw() {
		ImGui::End();
		ImGui::PopStyleVar();
	}
}