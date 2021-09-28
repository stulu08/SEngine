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
		
		m_scene->m_registry.each([&](auto goID) {
			GameObject gameObject{ goID, m_scene.get() };
			drawObject(gameObject);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_selectedObject = GameObject::null;

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

			m_scene->m_registry.view<TransformComponent>().each([=](entt::entity go, TransformComponent& transform) {
				if (transform.parent == m_selectedObject)
					transform.parent = GameObject::null;
			});

			m_selectedObject = GameObject::null;
		}
	}
	void EditorHierarchyPanel::drawObject(GameObject gameObject, int childIndex) {
		auto transform = gameObject.getComponent<TransformComponent>();
		if (transform.parent && childIndex == 0)
			return;
		GameObjectBaseComponent baseComp = gameObject.getComponent<GameObjectBaseComponent>();
		ImGui::SetCursorPosX(10.0f * childIndex);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (m_selectedObject == gameObject ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool open = ImGui::TreeNodeEx((void*)gameObject, flags, baseComp.name.c_str());
		if(ImGui::IsItemClicked())
			m_selectedObject = gameObject;

		//drag drop
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("GAMEOBJECT_HIERARCHY_MOVE", &gameObject, sizeof(GameObject));
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_HIERARCHY_MOVE")) {
				ST_ASSERT(payload->DataSize == sizeof(GameObject), "GAMEOBJECT_HIERARCHY_MOVE Drag Drop payload size wrong");
				GameObject go = *(const GameObject*)payload->Data;
				transform.addChild(go);
			}
			ImGui::EndDragDropTarget();
		}

		//end
		if (open) {
			m_scene->m_registry.view<TransformComponent>().each([=](entt::entity go, TransformComponent& transform) {
				if(transform.parent == gameObject)
					drawObject({ go, m_scene.get() }, childIndex + 1);
			});
			ImGui::TreePop();
		}
		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvailWidth(),1));
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GAMEOBJECT_HIERARCHY_MOVE")) {
				ST_ASSERT(payload->DataSize == sizeof(GameObject), "GAMEOBJECT_HIERARCHY_MOVE Drag Drop payload size wrong");
				GameObject go = *(const GameObject*)payload->Data;
				if (childIndex == 0)
					go.getComponent<TransformComponent>().parent = GameObject::null;
				else
					transform.parent.getComponent<TransformComponent>().addChild(go);
			}
			ImGui::EndDragDropTarget();
		}
	}
	void EditorHierarchyPanel::noScene() {
		ImGui::Text("No Scene loaded");
		ImGui::End();
	}
}