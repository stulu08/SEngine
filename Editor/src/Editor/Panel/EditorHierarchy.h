#pragma once
#include "EditorInspector.h"

namespace Stulu {
	class EditorHierarchyPanel {
	public:
		EditorHierarchyPanel() { m_scene = nullptr; };

		void setScene(const Ref<Scene>& scene);
		GameObject getCurrentObject() { return m_selectedObject; }
		void render();
	private:
		void drawObject(GameObject gameObject, int childIndex = 0);

		void noScene();

		Ref<Scene> m_scene;
		GameObject m_selectedObject = GameObject::null;

		EditorInspectorPanel m_inspectorPanel;

		friend class Scene;
	};
}