#include "EditorHierarchy.h"

#include <imgui/imgui.h>

namespace Stulu {
	void EditorHierarchyPanel::setScene(const Ref<Scene>& scene) {
		m_scene = scene;
	}

	void EditorHierarchyPanel::render() {
		ImGui::Begin("Hierarchy");
		if (m_scene == nullptr)
			return noScene();

		m_scene->m_registry.each([&](auto goID){
			GameObject gameObject{ goID, m_scene.get() };
			drawObject(gameObject);
		});
		if (ImGui::BeginPopupContextWindow("Create")) {
			if (ImGui::MenuItem("GameObject")) {
				m_scene->createGameObject();
			}
			if (ImGui::MenuItem("SpriteRenderer")) {
				m_scene->createGameObject("SpriteRenderer").addComponent<SpriteRendererComponent>();
			}
			if (ImGui::MenuItem("Directional Light")) {
				m_scene->createGameObject("Directional Light").addComponent<LightComponent>(LightComponent::Directional);
			}
			if (ImGui::MenuItem("Area Light")) {
				m_scene->createGameObject("Area Light").addComponent<LightComponent>(LightComponent::Area);
			}
			if (ImGui::MenuItem("Spot Light")) {
				m_scene->createGameObject("Spot Light").addComponent<LightComponent>(LightComponent::Spot);
			}
			if (ImGui::MenuItem("Orthographic Camera")) {
				GameObject g = m_scene->createGameObject("Orthographic Camera");
				g.addComponent<CameraComponent>(CameraMode::Orthographic);
				g.getComponent<GameObjectBaseComponent>().tag = "MainCam";
			}
			if (ImGui::MenuItem("Perspective Camera")) {
				GameObject g = m_scene->createGameObject("Perspective Camera");
				g.addComponent<CameraComponent>(CameraMode::Perspective);
				g.getComponent<GameObjectBaseComponent>().tag = "MainCam";
			}
			ImGui::EndPopup();
		}
		ImGui::End();
		if (!m_inspectorPanel.render(m_selectedObject)) {
			m_selectedObject = GameObject::null;
		}
	}
	void EditorHierarchyPanel::drawObject(GameObject gameObject) {
		GameObjectBaseComponent baseComp = gameObject.getComponent<GameObjectBaseComponent>();
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (m_selectedObject == gameObject ? ImGuiTreeNodeFlags_Selected : 0);

		bool open = ImGui::TreeNodeEx((void*)gameObject, flags, baseComp.name.c_str());
		if(ImGui::IsItemClicked())
			m_selectedObject = gameObject;
		if (open) {
			ImGui::TreePop();
		}
	}
	void EditorHierarchyPanel::noScene() {
		ImGui::Text("No Scene loaded");
		ImGui::End();
	}
}