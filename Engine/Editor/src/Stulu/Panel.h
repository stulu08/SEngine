#pragma once
#include <string>
#include <imgui/imgui.h>

namespace Editor {
	class Panel {
	public:
		inline Panel(const std::string windowName, bool defaultOpen = true, ImGuiWindowFlags windowFlags = 0)
			: m_displayName(windowName), m_open(defaultOpen), m_windowFlags(windowFlags) {}

		virtual ~Panel() = default;

		virtual void InvokeImGui();
		virtual void DrawImGui() = 0;
		virtual void DrawImGuizmo() = 0;

		virtual void PreWindow() {};
		virtual void PostWindow() {};

		inline bool& OpenPtr() { return m_open; }
		inline bool IsOpen() const { return m_open; }
		inline void SetOpen(bool value) { m_open = value; }
		inline const std::string& GetID() const { return m_displayName; }
		inline const std::string& GetDisplayName() const { return m_displayName; }
		inline ImGuiWindowFlags GetWindowFlags() const { return m_windowFlags; }
	private:
		bool m_open;
		std::string m_displayName;
		ImGuiWindowFlags m_windowFlags;
	};

	inline void Panel::InvokeImGui() {
		if (m_open) {
			PreWindow();
			if (ImGui::Begin(m_displayName.c_str(), &m_open, m_windowFlags)) {
				DrawImGui();
			}
			PostWindow();
			ImGui::End();
		}
		
	}
}