#pragma once
#include "Editor/Panel/EditorInspector.h"

namespace Stulu {
	class EditorHierarchyPanel {
	public:
		EditorHierarchyPanel() { m_scene = nullptr; };

		void setScene(const Ref<Scene>& scene);
		static inline GameObject& getCurrentObject() { return m_selectedObject; }
		static void setSelectedGameObject(GameObject object);
		void render(bool* open);
	private:
		void drawObject(GameObject gameObject, int childIndex = 0);

		void noScene();

		Ref<Scene> m_scene;
		static inline GameObject m_selectedObject = GameObject::null;

		GameObject m_lastHovered;
		float m_firstHovered;
		const float m_timeToHoverSelect = 1.0f;
	};
}