#pragma once
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

#include <functional>

namespace Editor {
	namespace Controls {
        extern std::string ControlHelpMark;

        inline void SetControlHelpMark(const std::string& text) {
            ControlHelpMark = text;
        }
        inline const std::string& GetControlHelpMark() {
            return ControlHelpMark;
        }
        inline void PopControlHelpMark() {
            ControlHelpMark.clear();
        }

        inline void Text(const std::string& text) {
            ImGui::TextWrapped(text.c_str());
        }
        inline void Text(const char* fmt, ...)
        {
            va_list args;
            va_start(args, fmt);
            ImGui::TextWrappedV(fmt, args);
            va_end(args);
        }

        inline bool LabeledBaseControl(const std::string& label, std::function<bool()> drawControl, float columnWidth = 0.0f) {
            ImGui::PushID(label.c_str());

            if (columnWidth == 0) {
                columnWidth = ImGui::GetContentRegionAvail().x / 2.0f;
            }

            ImGui::Columns(2, 0, false);

            // col 1
            ImGui::SetColumnWidth(0, columnWidth);

            ImGui::PushTextWrapPos(columnWidth);
            ImGui::TextWrapped("%s", label.c_str());
            ImGui::PopTextWrapPos();

            if (!GetControlHelpMark().empty()) {
                ImGui::SameLine();
                ImGui::HelpMarker(GetControlHelpMark().c_str());
            }

            ImGui::NextColumn();
            // col 2
            bool changed = drawControl();

            ImGui::Columns(1);
            ImGui::PopID();

            return changed;
        }
	}
}