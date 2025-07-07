#include "LogPanel.h"

#include "Stulu/Style.h"
using namespace Stulu;

namespace Editor {
	std::vector<LogEntry> LogPanel::s_entries;
	bool LogPanel::s_alert = false;

	LogPanel::LogPanel()
		: m_showInfo(true), m_showWarn(true), m_showError(true), m_clearOnPlay(true), Panel("Log") {

        App::get().GetPrefs().Section("Log");
        m_clearOnPlay = App::get().GetPrefs().GetAs<bool>("ClearOnPlay", true);
	}
    
    LogPanel::~LogPanel() {
        App::get().GetPrefs().Section("Log");
        App::get().GetPrefs().Set("ClearOnPlay", m_clearOnPlay);
    }

    void LogPanel::Update() {
        if (s_alert) {
            //ImGui::SetWindowFocus(GetDisplayName().c_str());
            s_alert = false;
        }
    }
	void LogPanel::DrawImGui() {
        

		if (ImGui::Button("Clear")) {
			Clear();
		}
        ImGui::SameLine();
        if (m_clearOnPlay) {
            if (ImGui::Button(ICON_FK_CHECK " Clear on Play")) {
                m_clearOnPlay = !m_clearOnPlay;
            }
        }
        else if(ImGui::Button("Clear on Play")) {
            m_clearOnPlay = !m_clearOnPlay;
        }


		static ImGuiTableFlags flags =
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_ScrollY |
			ImGuiTableFlags_RowBg |
			ImGuiTableColumnFlags_WidthFixed;

        ImVec2 outer_size = ImGui::GetContentRegionAvail();
        if (ImGui::BeginTable("LogTable", 3, flags, outer_size)) {

            ImGui::TableSetupScrollFreeze(0, 1);

            float totalWidth = outer_size.x;
            float col1 = totalWidth * (1.0f / 12.0f);
            float col2 = totalWidth * (1.0f / 12.0f);
            float col3 = totalWidth * (5.0f / 6.0f);

            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch, col1);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthStretch, col2);
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch, col3);
            ImGui::TableHeadersRow();

            for (const auto& entry : s_entries) {
                DrawEntry(entry);
            }

            ImGui::EndTable();
        }
	}

	void LogPanel::DrawEntry(const LogEntry& entry) const{
        ImGui::TableNextColumn();
		
        switch (entry.level)
        {
        case Stulu::Log::Level::trace:
        {
            ImVec4 color = ImVec4(0.4f, 0.8f, 0.9f, 1.0f); // soft cyan
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted("Trace");
            ImGui::PopStyleColor();
            break;
        }
        case Stulu::Log::Level::info:
        {
            ImGui::TextUnformatted("Info"); // default color
            break;
        }
        case Stulu::Log::Level::warn:
        {
            ImVec4 color = ImVec4(0.95f, 0.75f, 0.2f, 1.0f); // soft yellow-orange
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted("Warn");
            ImGui::PopStyleColor();
            break;
        }
        case Stulu::Log::Level::error:
        {
            ImVec4 color = ImVec4(0.9f, 0.3f, 0.3f, 1.0f); // light red
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted("Error");
            ImGui::PopStyleColor();
            break;
        }
        case Stulu::Log::Level::critical:
        {
            ImVec4 color = ImVec4(0.6f, 0.1f, 0.1f, 1.0f); // dark red
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted("Critical");
            ImGui::PopStyleColor();
            break;
        }
        }

		ImGui::TableNextColumn();

		auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
		std::tm tm = *std::localtime(&time);
		char time_buf[64];
		strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm);
		ImGui::TextUnformatted(time_buf);


		ImGui::TableNextColumn();
		ImGui::TextUnformatted(entry.msg.c_str());
	}
}