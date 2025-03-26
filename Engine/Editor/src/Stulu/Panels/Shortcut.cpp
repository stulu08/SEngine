#include "Shortcut.h"

#include "Stulu/App.h"
#include "Stulu/Controls.h"

namespace Editor {
	ShortcutPanel::ShortcutPanel() 
		: Panel("Shortcuts", false) {

		LoadShortcuts();
	}
	void ShortcutPanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Shortcut");

		for (auto& [groupName, shortcutEntry] : m_groups) {
			if (ImGui::TreeNodeEx(groupName.c_str())) {
				for (const auto& entry : shortcutEntry) {
					if (!entry.shortcut)
						return;

					Controls::Text("%s:\t%s", entry.name.c_str(), entry.shortcut->BuildKeyString().c_str());
				}
				ImGui::TreePop();
			}
		}
	}

	void ShortcutPanel::LoadShortcuts() {
		auto& layer = App::get().GetLayer();

		m_groups.clear();

		for (auto& shortcut : layer.GetShortcuts()) {
			const std::string name = shortcut.GetName();
			const size_t searchPos = name.find_first_of("-");

			if (searchPos == name.npos) {
				m_groups["General"].push_back({ name, &shortcut });
			}
			else {
				const std::string group = name.substr(0, searchPos);
				const std::string clearName = name.substr(searchPos + 1);
				m_groups[group].push_back({ clearName, &shortcut });
			}
		}
	}
}

