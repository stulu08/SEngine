#pragma once
#include "EditorInspector.h"

namespace Stulu {
	class EditorHierarchyPanel {
	public:
		EditorHierarchyPanel() { m_scene = nullptr; };

		void setScene(const Ref<Scene>& scene);

		void render();
	private:
		void drawObject(GameObject gameObject);

		void noScene();

		Ref<Scene> m_scene;
		GameObject m_selectedObject = GameObject::null;

		EditorInspectorPanel m_inspectorPanel;

		friend class Scene;
	};
}