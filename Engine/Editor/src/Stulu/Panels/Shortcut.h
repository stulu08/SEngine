#pragma once
#include "Stulu/Panel.h"

#include "Stulu/MainLayer.h"

namespace Editor {
	struct ShortcutEntry {
		std::string name;
		Shortcut* shortcut;
	};

	using ShortcutGroup = std::vector<ShortcutEntry>;
	using ShortcutGroupList = std::unordered_map<std::string, ShortcutGroup>;

	class ShortcutPanel : public Panel {
	public:
		ShortcutPanel();
		virtual ~ShortcutPanel() = default;

		virtual void DrawImGui() override;
	private:
		ShortcutGroupList m_groups;

		void LoadShortcuts();
	};
}


