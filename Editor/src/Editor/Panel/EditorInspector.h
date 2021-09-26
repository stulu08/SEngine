#pragma once
#include "ComponentsRender.h"
namespace Stulu {
	class EditorInspectorPanel {
	public:
		EditorInspectorPanel() = default;

		bool render(GameObject gameObject);
	private:
		template <typename T>
		void drawComponent(GameObject gameObject, std::string name, bool removeable = true);
	};
}