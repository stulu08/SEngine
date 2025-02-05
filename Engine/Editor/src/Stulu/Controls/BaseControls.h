#pragma once
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <imgui_internal.h>

#include <functional>

namespace Editor {
	namespace Controls {
        inline bool LabeledBaseControl(const std::string& label, std::function<bool()> drawControl, float columnWidth = 0.0f) {
            ImGui::PushID(label.c_str());

            if (columnWidth == 0) {
                columnWidth = ImGui::GetContentRegionMax().x / 3.0f * 2.0f;
            }
            ImGui::Columns(2, 0, false);
            ImGui::SetColumnWidth(0, columnWidth);
            bool changed = drawControl();

            ImGui::NextColumn();
            ImGui::Text("%s", label.c_str());

            ImGui::Columns(1);
            ImGui::PopID();

            return changed;
        }
	}
}