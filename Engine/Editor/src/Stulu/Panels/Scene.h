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

		inline Stulu::SceneCamera& GetCamera() {
			return m_sceneCamera;
		}
		inline bool IsVisible() const {
			return IsOpen();
		}
		inline bool IsFocused() const {
			return m_focused;
		}
		inline uint32_t GetWidth() const { return m_width; }
		inline uint32_t GetHeight() const { return m_height; }

		inline Stulu::GizmoTransformEditMode GetGizmoEditMode() const {
			return (Stulu::GizmoTransformEditMode)m_gizmoMode;
		}
	private:
		ImVec2 m_windowPos = ImVec2(0, 0);
		Stulu::SceneCamera m_sceneCamera;
		bool m_focused = false;
		uint32_t m_width = 1, m_height = 1;
		uint32_t m_gizmoMode = (uint32_t)Stulu::GizmoTransformEditMode::None;
		bool m_drawGrid = false;
	};
}