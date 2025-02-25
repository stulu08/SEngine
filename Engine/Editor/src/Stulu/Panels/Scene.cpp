#include "Scene.h"
#include <imgui_internal.h>
#include <Stulu/ImGui/Gizmo.h>

using namespace Stulu;

namespace Editor {
	ScenePanel::ScenePanel()
	: m_sceneCamera(0.0, 85.0f, .001f, 1000.0f, 1), Panel("Scene") {

	}
	void ScenePanel::DrawImGui() {
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_width = (uint32_t)glm::max(viewportSize.x, 1.0f);
		m_height = (uint32_t)glm::max(viewportSize.y, 1.0f);

		FrameBufferSpecs FBspec = m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs();
		if (FBspec.width != m_width || FBspec.height != m_height)
			m_sceneCamera.onResize((float)m_width, (float)m_height);

		m_focused = ImGui::IsWindowFocused();

		Gizmo::ApplyToFrameBuffer(m_sceneCamera.getCamera()->getFrameBuffer());

		auto& texture = m_sceneCamera.getCamera()->getFrameBuffer()->getColorAttachment();
		ImGui::Image(texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));

		


		glm::vec2 windowPos = glm::vec2(ImGui::GetCurrentWindow()->WorkRect.Min.x, ImGui::GetCurrentWindow()->WorkRect.Min.y);
		Gizmo::setRect(windowPos.x, windowPos.y, (float)m_width, (float)m_height);
		Gizmo::setCamData(m_sceneCamera.getCamera()->getProjectionMatrix(), glm::inverse(m_sceneCamera.getTransform().transform));
	}
	void ScenePanel::DrawImGuizmo() {	}


	void ScenePanel::OnEvent(Stulu::Event& e) {
		if(m_focused)
			m_sceneCamera.onEvent(e);
	}
	bool ScenePanel::BeginGizmo() {
		m_gizmosVisible = false;
		m_gizmosWindowVisible = false;

		if (this->IsOpen()) {
			m_gizmosWindowVisible = true;
			if (ImGui::Begin("Scene", &OpenPtr())) {
				Gizmo::Begin();
				m_gizmosVisible = true;
				return true;
			}
		}
		return false;
	}
	void ScenePanel::EndGizmo() {
		if (m_gizmosVisible)
			Gizmo::End();

		if (m_gizmosWindowVisible) {
			ImGui::End();
		}
	}
}
