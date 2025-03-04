#pragma once
#include "Stulu/Panel.h"
#include <Stulu/Scene/SceneCamera.h>
#include <Stulu/ImGui/Gizmo.h>


namespace Editor {
	class ScenePanel : public Panel {
	public:
		ScenePanel();

		virtual void DrawImGui() override;
		virtual void DrawImGuizmo() override;
		virtual void Update() override;
		virtual void OnEvent(Stulu::Event& e) override;

		virtual void PostWindow() override;
		virtual void PreWindow() override;

		inline Stulu::SceneCamera& GetCamera() {
			return m_sceneCamera;
		}
		inline uint32_t GetWidth() const { return m_width; }
		inline uint32_t GetHeight() const { return m_height; }

		inline Stulu::GizmoTransformEditMode GetGizmoEditMode() const {
			return (Stulu::GizmoTransformEditMode)m_gizmoMode;
		}

		void DrawMenuBars(ImVec2 startPos, bool showToolbar = true);
	private:
		ImVec2 m_windowPos = ImVec2(0, 0);
		ImVec2 m_windowPadding = ImVec2(6.0f, 8.0f);
		Stulu::SceneCamera m_sceneCamera;
		uint32_t m_width = 1, m_height = 1;
		uint32_t m_gizmoMode = (uint32_t)Stulu::GizmoTransformEditMode::None;
		bool m_drawGrid = false;
	};
}