#pragma once
#include "Stulu/Panel.h"

namespace Editor {
	class SettingsPanel : public Panel {
	public:
		SettingsPanel();
		virtual ~SettingsPanel() = default;

		virtual void DrawImGui() override;
	private:

		void DrawPhysics();
		void DrawGraphics();
	};
}


