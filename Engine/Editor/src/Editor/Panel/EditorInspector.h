#pragma once
#include "Editor/Panel/ComponentsRender.h"
namespace Stulu {
	class EditorInspectorPanel {
	public:
		EditorInspectorPanel() = default;

		bool render(bool* open);

		void renderGameObjectInspector();
		void renderAssetInspector();


		static inline bool AssetMode = false;
	private:
		template <typename T>
		void drawComponent(GameObject gameObject, std::string name, bool removeable = true);
		void drawScriptingComponent(GameObject gameObject);
	};
}