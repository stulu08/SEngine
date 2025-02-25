#pragma once
#include "Stulu/Panel.h"
#include <Stulu/Scene/SceneCamera.h>


namespace Editor {
	class ScenePanel : public Panel {
	public:
		ScenePanel();

		virtual void DrawImGui() override;
		virtual void DrawImGuizmo() override;
		virtual void OnEvent(Stulu::Event& e) override;

		inline Stulu::SceneCamera& GetCamera() {
			return m_sceneCamera;
		}
		inline bool IsVisible() const {
			return IsOpen();
		}
		inline bool IsFocused() const {
			return m_focused;
		}

		bool BeginGizmo();
		void EndGizmo();
	private:
		Stulu::SceneCamera m_sceneCamera;
		bool m_focused = false;
		uint32_t m_width = 1, m_height = 1;

		bool m_gizmosVisible = false;
		bool m_gizmosWindowVisible = false;

	};
}