#include "Hierarchy.h"
#include "Stulu/Controls.h"

using namespace Stulu;

namespace Editor {
	HierarchyPanel::HierarchyPanel(Ref<Scene> scene) 
		: Panel("Hierarchy"), m_scene(scene.get()) {}

	void HierarchyPanel::PreWindow() {
		m_windowPaddingOriginal = ImGui::GetStyle().WindowPadding.x;
		ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
	}

	void HierarchyPanel::PostWindow() {
		ImGui::PopStyleVar(2);
	}

	size_t CountChildren(entt::entity enID, Scene* scene);

	void HierarchyPanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Hierarchy");

		// search bar
		ImGuiStyle& style = ImGui::GetStyle();
		float fullWidth = ImGui::GetContentRegionAvail().x - (2 * m_windowPaddingOriginal);
		ImGui::SetNextItemWidth(fullWidth);
		ImGui::SetCursorPosX(m_windowPaddingOriginal);
		ImGui::InputTextWithHint("##SearchBar", ICON_FK_SEARCH " Search...", & m_search);
		

		static ImGuiTableFlags flags =
			ImGuiTableFlags_BordersInner |
			ImGuiTableFlags_ScrollX |
			ImGuiTableFlags_ScrollY |
			ImGuiTableFlags_SizingFixedSame;

		ImVec2 outer_size = ImGui::GetContentRegionAvail();
		if (ImGui::BeginTable("EditorHierarchy", 3, flags, outer_size)) {

			ImGui::TableSetupScrollFreeze(0, 1);

			float totalWidth = outer_size.x;
			float col1 = totalWidth * (5.0f / 6.0f);
			float col2 = totalWidth * (3.0f / 12.0f);
			float col3 = totalWidth * (2.0f / 12.0f);

			ImGui::TableSetupColumn("\tName", ImGuiTableColumnFlags_WidthStretch, col1);
			ImGui::TableSetupColumn("Tag", ImGuiTableColumnFlags_WidthStretch, col2);
			ImGui::TableSetupColumn(ICON_FK_EYE, ImGuiTableColumnFlags_WidthStretch, col3);
			ImGui::TableHeadersRow();

			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
			ImGui::PushStyleVarY(ImGuiStyleVar_CellPadding, 0.0f);

			if (m_scene) {
				for (auto& [gameObjectID, baseComponent] : GetRegistry().storage<GameObjectBaseComponent>().each()) {
					GameObject gameObject = baseComponent.gameObject;
					if (!gameObject.IsValid())
						continue;
					DrawGameObject(baseComponent);
				}
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			if (ImGui::IsWindowHovered() && (ImGui::IsMouseReleased(0) || ImGui::IsMouseReleased(1))) {
				// 1 frame delay
				if (ImGui::TableGetHoveredRow() == -1)
					m_selected.clear();

			}

			ImGui::EndTable();


			// menu
			ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, m_windowPaddingOriginal);
			if (ImGui::BeginPopupContextItem("HIERARCHY_CONTEXT_MENU")) {

				GameObject parent = GameObject::null;
				if (m_selected.size() == 1) {
					parent = { m_selected[0], m_scene };
				}

				if (ImGui::MenuItem("GameObject")) {
					GameObject go = m_scene->Create("New GameObject");
				}

				if (ImGui::BeginMenu("3D")) {
					if (ImGui::MenuItem("Cube")) {
						GameObject go = m_scene->Create("New Cube");
						go.addComponent<MeshFilterComponent>().SetMesh(Stulu::Resources::CubeMesh());
						go.addComponent<MeshRendererComponent>();
						go.addComponent<BoxColliderComponent>();

						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					if (ImGui::MenuItem("Plane")) {
						GameObject go = m_scene->Create("New Plane");
						go.addComponent<MeshFilterComponent>().SetMesh(Stulu::Resources::PlaneMesh());
						go.addComponent<MeshRendererComponent>();
						go.addComponent<BoxColliderComponent>().SetSize(glm::vec3(.5f, .01f, .5f));
						go.getComponent<BoxColliderComponent>().SetOffset(glm::vec3(.0f, -.01f, .0f));
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					if (ImGui::MenuItem("Sphere")) {
						GameObject go = m_scene->Create("New Sphere");
						go.addComponent<MeshFilterComponent>().SetMesh(Stulu::Resources::SphereMesh());
						go.addComponent<MeshRendererComponent>();
						go.addComponent<SphereColliderComponent>();
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					if (ImGui::MenuItem("Capsule")) {
						GameObject go = m_scene->Create("New Capsule");
						go.addComponent<MeshFilterComponent>().SetMesh(Stulu::Resources::CapsuleMesh());
						go.addComponent<MeshRendererComponent>();
						go.addComponent<CapsuleColliderComponent>();
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("2D")) {
					if (ImGui::MenuItem("Quad/Sprite")) {
						GameObject go = m_scene->Create("New Sprite");
						go.addComponent<SpriteRendererComponent>();
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					if (ImGui::MenuItem("Circle")) {
						GameObject go = m_scene->Create("New Circle");
						go.addComponent<CircleRendererComponent>();
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Rendering")) {
					if (ImGui::MenuItem("3D Camera")) {
						GameObject go = m_scene->Create("New Camera");
						go.addComponent<CameraComponent>(CameraMode::Perspective, m_scene->getViewportWidth(), m_scene->getViewportHeight());
						go.addComponent<PostProcessingComponent>();
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					if (ImGui::MenuItem("2D Camera")) {
						GameObject go = m_scene->Create("New Camera");
						go.addComponent<CameraComponent>(CameraMode::Orthographic, m_scene->getViewportWidth(), m_scene->getViewportHeight());
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					ImGui::Separator();
					if(ImGui::MenuItem("Directional Light")) {
						GameObject go = m_scene->Create("New Directional Light");
						go.addComponent<LightComponent>(LightComponent::Directional);
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					if (ImGui::MenuItem("Spot Light")) {
						GameObject go = m_scene->Create("New Spot Light");
						go.addComponent<LightComponent>(LightComponent::Spot);
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}
					if (ImGui::MenuItem("Area Light")) {
						GameObject go = m_scene->Create("New Area Light");
						go.addComponent<LightComponent>(LightComponent::Area);
						if (parent) go.getComponent<TransformComponent>().SetParent(parent);
					}

					ImGui::EndMenu();
				}


				if (m_selected.size() > 0) {
					ImGui::Separator();

					// more then 0 object
					if (m_selected.size() > 1) {
						size_t count = 0;
						for (entt::entity enID : m_selected) {
							count += CountChildren(enID, m_scene);
						}
						std::string deleteDisplay = "Delete (" + std::to_string(count) + " Objects)";
						if (ImGui::MenuItem(deleteDisplay.c_str())) {
							for (entt::entity enID : m_selected) {
								m_scene->Destroy({ enID, m_scene });
							}
							m_selected.clear();
						}
					}
					// selected.size == 1
					else {
						GameObject selected = { m_selected[0], m_scene };

						if (ImGui::MenuItem("Add Child")) {
							selected.getComponent<TransformComponent>().AddChild(m_scene->Create("New Child"));
						}

						std::string deleteDisplay = selected.getComponent<TransformComponent>().HasChildren() ?
							"Delete (" + std::to_string(CountChildren(selected.GetID(), m_scene)) + " Objects)" : "Delete";

						if (ImGui::MenuItem(deleteDisplay.c_str())) {
							m_scene->Destroy(selected);
							m_selected.clear();
						}
					}
				}
			   

				
				ImGui::EndPopup();
			}
			ImGui::PopStyleVar();
		}

	}

	size_t CountChildren(entt::entity enID, Scene* scene) {
		size_t count = 0;

		GameObject go = { enID, scene };
		if (go.hasComponent<TransformComponent>()) {
			TransformComponent& tc = go.getComponent<TransformComponent>();
			for (entt::entity childID : tc.GetChildren())
				count += CountChildren(childID, scene);
		}
		return count + 1;
	}

	void HierarchyPanel::DrawImGuizmo(){}

	void HierarchyPanel::DrawGameObject(GameObjectBaseComponent& baseComponent, const GameObject& parent) {
		GameObject gameObject = baseComponent.gameObject;

		// return if it is child of someone
		TransformComponent& transform = gameObject.getComponent<TransformComponent>();
		if (transform.GetParent() != parent)
			return;

		ImGui::TableNextColumn();

		// drag drop to same level
		DummyDragDropTarget(gameObject);

		// tree node
		ImGuiTreeNodeFlags treeFlags =
			ImGuiTreeNodeFlags_SpanFullWidth |
			ImGuiTreeNodeFlags_SpanAllColumns |
			ImGuiTreeNodeFlags_FramePadding |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_OpenOnArrow;

		if (IsSelected(gameObject.GetID()))
			treeFlags |= ImGuiTreeNodeFlags_Selected;
		if(!transform.HasChildren())
			treeFlags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::SetNextItemAllowOverlap();

		// position of the arrow 
		const float touchExtraPadding = ImGui::GetStyle().TouchExtraPadding.x;
		float treeArrowX1 = ImGui::GetCursorScreenPos().x - touchExtraPadding;
		float treeArrowX2 = treeArrowX1 + ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.x * 2.0f + touchExtraPadding * 2.0f;

		const std::string& objectName = baseComponent.name.c_str();
		ImGuiID TreeNodeID = ImGui::GetCurrentWindow()->GetID(objectName.c_str());
		bool treeOpen = ImGui::TreeNodeEx(objectName.c_str(), treeFlags);

		// drag drop to child
		DragDropSource();
		DragDropTarget(gameObject);

		if (Controls::DragDropHoverBtnPressed()) {
			// if arrow hovered open, otherwise select
			float mouseX = ImGui::GetMousePos().x;
			if (mouseX >= treeArrowX1 && mouseX <= treeArrowX2 && transform.HasChildren()) {
				ImGui::TreeNodeSetOpen(TreeNodeID, !ImGui::TreeNodeGetOpen(TreeNodeID));
			}
			else {
				SetSelected(gameObject.GetID());
			}
		}
		// selecting
		else if (ImGui::IsItemHovered()) {
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				SelectedLogicAdd(gameObject.GetID());
			}
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
				if (IsSelected(gameObject.GetID())) {
					SelectedLogicAdd(gameObject.GetID());
				}
				else {
					if (m_selected.size() < 1) {
						SetSelected(gameObject.GetID());
					}
					else {
						m_selected.clear();
					}
				}
			}
		}

		// tag
		ImGui::TableNextColumn();

		ImGui::Text(baseComponent.tag.c_str());

		// visible
		ImGui::TableNextColumn();
		bool value = true;
		ImGui::Text(ICON_FK_EYE "");

		// draw children
		if (treeOpen) {
			for (entt::entity& childObj : transform.GetChildren()) {
				GameObject child = { childObj, m_scene };
				auto& childBaseComponent = child.getComponent<GameObjectBaseComponent>();
				if (!childBaseComponent.gameObject.IsValid())
					continue;
				DrawGameObject(childBaseComponent, gameObject);
			}

			ImGui::TreePop();
		}
	}

	void HierarchyPanel::DummyDragDropTarget(const Stulu::GameObject& parent) {
		float posY = ImGui::GetCursorPosY();
		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 2));
		DragDropTarget(parent.getComponent<TransformComponent>().GetParent());
		ImGui::SetCursorPosY(posY);
	}

	void HierarchyPanel::DragDropTarget(const Stulu::GameObject& parent) {
		std::vector<entt::entity> list;
		if (Controls::ReceiveDragDopGameObjects(list)) {
			SetParents(parent, list);
		}
	}

	void HierarchyPanel::DragDropSource() {
		Controls::DragDropGameObjects(m_selected);
	}

	void HierarchyPanel::SetParents(const Stulu::GameObject& parent, const std::vector<entt::entity> children) {
		for (entt::entity child : children) {
			GameObject target = { child, m_scene };
			if (target) {

				// Prevent circular parent assignment
				GameObject ancestor = parent;
				while (ancestor) {
					if (ancestor == target) {
						return;  // Stop assignment to prevent cycles
					}
					ancestor = ancestor.getComponent<TransformComponent>().GetParent();
				}

				target.getComponent<TransformComponent>().SetParent(parent);
			}
		}
	}

}
