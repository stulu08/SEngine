#pragma once
#include <string>
#include <imgui/imgui.h>

#include <Stulu/Debug/Profiler.h>
#include <Stulu/Events/Event.h>

namespace Editor {
	class Panel {
	public:
		inline Panel(const std::string windowName, bool defaultOpen = true, ImGuiWindowFlags windowFlags = 0)
			: m_displayName(windowName), m_open(defaultOpen), m_windowFlags(windowFlags) {}

		virtual ~Panel() = default;

		virtual void InvokeImGui();
		virtual void DrawImGui() = 0;
		virtual void DrawImGuizmo() {};

		virtual void OnEvent(Stulu::Event& e) {};

		virtual void PreWindow() {};
		virtual void PostWindow() {};
		virtual void Update() {};

		inline bool& OpenPtr() { return m_open; }
		inline bool IsFocused() const { return m_focused; }
		inline bool IsVisible() const { return m_visible; }
		inline bool IsOpen() const { return m_open; }
		inline void SetOpen(bool value) { m_open = value; }
		inline const std::string& GetID() const { return m_displayName; }
		inline const std::string& GetDisplayName() const { return m_displayName; }
		inline ImGuiWindowFlags GetWindowFlags() const { return m_windowFlags; }
	private:
		bool m_open, m_focused = false, m_visible = false;
		std::string m_displayName;
		ImGuiWindowFlags m_windowFlags;
	};

	inline void Panel::InvokeImGui() {
		m_focused = m_visible = false;
		if (m_open) {
			PreWindow();
			if (ImGui::Begin(m_displayName.c_str(), &m_open, m_windowFlags)) {
				m_focused = ImGui::IsWindowFocused();
				m_visible = true;

				DrawImGui();
			}
			ImGui::End();
			PostWindow();
		}
		
	}
}