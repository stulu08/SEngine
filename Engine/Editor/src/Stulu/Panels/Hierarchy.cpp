#include "Hierarchy.h"
#include <Icons.h>


using namespace Stulu;

namespace Editor {
	HierarchyPanel::HierarchyPanel(Ref<Scene> scene) 
		: Panel("Hierarchy"), m_scene(scene.get()) {}

	void HierarchyPanel::DrawImGui() {
        // load for a maximum ammount of childs
        m_childObjectsBuffer.reserve(GetRegistry().storage<entt::entity>().in_use());

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
            ImGui::TableSetupColumn("Visibility", ImGuiTableColumnFlags_WidthStretch, col3);
            ImGui::TableHeadersRow();

            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));
            ImGui::PushStyleVarY(ImGuiStyleVar_CellPadding, 0.0f);

            if (m_scene) {
                for (auto& [gameObjectID, baseComponent] : GetRegistry().storage<GameObjectBaseComponent>().each()) {
                    GameObject gameObject = baseComponent.gameObject;
                    if (!gameObject.isValid())
                        continue;
                    DrawGameObject(baseComponent);
                }
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            ImGui::EndTable();
        }
	}

	void HierarchyPanel::DrawImGuizmo(){}

    void HierarchyPanel::DrawGameObject(const GameObjectBaseComponent& baseComponent, const GameObject& parent) {
        GameObject gameObject = baseComponent.gameObject;

        // return if it is child of someone
        TransformComponent& transform = gameObject.getComponent<TransformComponent>();
        if (transform.parent != parent)
            return;

        LoadChilds(gameObject);

        ImGui::TableNextColumn();

        // drag drop to same level
        DummyDragDropTarget(gameObject);

        // tree node
        ImGuiTreeNodeFlags treeFlags =
            ImGuiTreeNodeFlags_SpanFullWidth |
            ImGuiTreeNodeFlags_SpanAllColumns |
            ImGuiTreeNodeFlags_FramePadding |
            ImGuiTreeNodeFlags_OpenOnArrow;

        if (IsSelected(gameObject))
            treeFlags |= ImGuiTreeNodeFlags_Selected;
        if(m_childObjectsBuffer.size() < 1)
            treeFlags |= ImGuiTreeNodeFlags_Leaf;

        bool treeOpen = ImGui::TreeNodeEx(baseComponent.name.c_str(), treeFlags);

        // drag drop to child
        DragDropSource();
        DragDropTarget(gameObject);

        // selecting
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
                AddSelected(gameObject);
            }
            else {
                SetSelected(gameObject);
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
            for (GameObject& child : m_childObjectsBuffer) {
                auto& childBaseComponent = child.getComponent<GameObjectBaseComponent>();
                if (!childBaseComponent.gameObject.isValid())
                    continue;
                DrawGameObject(childBaseComponent, gameObject);
            }

            ImGui::TreePop();
        }
	}

    void HierarchyPanel::DummyDragDropTarget(const Stulu::GameObject& parent) {
        float posY = ImGui::GetCursorPosY();
        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 2));
        DragDropTarget(parent.getComponent<TransformComponent>().parent);
        ImGui::SetCursorPosY(posY);
    }

    void HierarchyPanel::DragDropTarget(const Stulu::GameObject& parent) {
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_GO_LIST")) {
                SetParents(parent, (entt::entity*)payload->Data, static_cast<size_t>(payload->DataSize / sizeof(entt::entity)));
            }
            ImGui::EndDragDropTarget();
        }
    }

    void HierarchyPanel::DragDropSource() {
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("SCENE_GO_LIST", m_selected.data(), sizeof(entt::entity) * m_selected.size());
            ImGui::EndDragDropSource();
        }
    }

	void HierarchyPanel::LoadChilds(const Stulu::GameObject& parent) {
        m_childObjectsBuffer.clear();

        for (auto& [childID, childTransform] : GetRegistry().storage<TransformComponent>().each()) {
            GameObject childObject = { childID, m_scene };
            if (!childObject.isValid())
                continue;
            if (childTransform.parent == parent)
                m_childObjectsBuffer.push_back(childObject);
        }
	}

	void HierarchyPanel::SetParents(const Stulu::GameObject& parent, entt::entity* targets, size_t count) {
		if (count < 1)
			return;
		for (size_t i = 0; i < count; i++) {
			entt::entity targetID = *(targets + i);
			GameObject target = { targetID, m_scene };
			if (target) {
				target.getComponent<TransformComponent>().parent = parent;
			}
		}
	}

}
