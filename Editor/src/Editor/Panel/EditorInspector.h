#pragma once
#include "Editor/Panel/ComponentsRender.h"
namespace Stulu {
	class EditorInspectorPanel {
	public:
		EditorInspectorPanel() = default;

		bool render(GameObject gameObject, bool* open);
	private:
		template <typename T>
		void drawComponent(GameObject gameObject, std::string name, bool removeable = true);
	};
}