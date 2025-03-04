#include "Hierarchy.h"
#include "Stulu/Controls.h"

using namespace Stulu;

namespace Editor {
	HierarchyPanel::HierarchyPanel(Ref<Scene> scene) 
		: Panel("Hierarchy"), m_scene(scene.get()) {}

    void HierarchyPanel::PreWindow() {
        m_windowPaddingOriginal = ImGui::GetStyle().WindowPadding.x;
        ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, 0.0f);
    }

    void HierarchyPanel::PostWindow() {
        ImGui::PopStyleVar();
    }

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
            ImGuiTreeNodeFlags_OpenOnArrow;

        if (IsSelected(gameObject))
            treeFlags |= ImGuiTreeNodeFlags_Selected;
        if(!transform.HasChildren())
            treeFlags |= ImGuiTreeNodeFlags_Leaf;

        ImGui::SetNextItemAllowOverlap();

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
            for (entt::entity& childObj : transform.GetChildren()) {
                GameObject child = { childObj, m_scene };
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
