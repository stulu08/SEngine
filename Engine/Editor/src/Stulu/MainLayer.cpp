#include "MainLayer.h"

#include "Controls.h"
#include "App.h"

using namespace Stulu;

namespace Editor {
	MainLayer::MainLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f, .001f, 1000.0f, 1) {
	}

	MainLayer::~MainLayer()
	{
	}

	void MainLayer::onAttach() {
		ImGui::SetCurrentContext(Application::get().getImGuiLayer()->getContext());

        const float fontSize = 16.0f;
        const float iconFontSize = fontSize - 1.0f; 

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("Data/Stulu/Editor/Fonts/EditorFont.ttf", fontSize);

        static const ImWchar icons_ranges[] = { ICON_MIN_FK, ICON_MAX_16_FK, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.GlyphMinAdvanceX = fontSize;
        icons_config.PixelSnapH = true;
        ImFont* iconFont = io.Fonts->AddFontFromFileTTF("Data/Stulu/Editor/Fonts/EditorIcons.ttf", iconFontSize, &icons_config, icons_ranges);


        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Colors
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colors[ImGuiCol_Border] = ImVec4(0.39f, 0.39f, 0.39f, 0.59f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.30f, 0.54f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);
        // Corners
        style.WindowPadding = ImVec2(0.00f, 0.00f);
        style.FramePadding = ImVec2(5.00f, 2.00f);
        style.CellPadding = ImVec2(6.00f, 6.00f);
        style.ItemSpacing = ImVec2(6.00f, 6.00f);
        style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
        style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
        style.IndentSpacing = 25;
        style.ScrollbarSize = 15;
        style.GrabMinSize = 10;
        style.WindowBorderSize = 1;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 1;
        style.TabBorderSize = 1;
        style.WindowRounding = 7;
        style.ChildRounding = 4;
        style.FrameRounding = 3;
        style.PopupRounding = 4;
        style.ScrollbarRounding = 9;
        style.GrabRounding = 3;
        style.LogSliderDeadzone = 4;
        style.TabRounding = 4;

        const std::string path = Editor::App::get().GetProject().GetAssetPath() + "/scenes/Sponza.scene";
        m_activeScene = createRef<Scene>();
        SceneSerializer ss(m_activeScene);
        if (ss.deSerialze(path)) {
            m_activeScene->onViewportResize(100, 100);
            ST_TRACE("Opened Scene {0}", path);
        }
	}
	void MainLayer::onUpdate(Timestep timestep)
	{
	}

    std::vector<GameObject> childObjects;
    void DrawGameObject(GameObjectBaseComponent& baseComponent, entt::registry& registry, const GameObject& parent = GameObject::null) {
        GameObject gameObject = baseComponent.gameObject;

        // return if it is child
        const TransformComponent& transform = gameObject.getComponent<TransformComponent>();
        if (transform.parent != parent)
            return;

        // find all childs
        childObjects.clear();
        for (auto& [childID, childTransform] : registry.storage<TransformComponent>().each()) {
            GameObject childObject = { childID, gameObject.getScene()};
            if (!childObject.isValid())
                continue;
            if (childTransform.parent == gameObject)
                childObjects.push_back(childObject);
        }
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        bool drawChilds = false;
        if (childObjects.size() > 0) {
            drawChilds = ImGui::TreeNodeEx(baseComponent.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
        }
        else {
            ImGui::TreeNodeEx(baseComponent.name.c_str(), ImGuiTreeNodeFlags_Leaf);
        }

        ImGui::TableNextColumn();
        ImGui::Text(baseComponent.tag.c_str());
        ImGui::TableNextColumn();
        bool value = true;
        ImGui::Checkbox("##enabled_checkbox", &value);

        if (drawChilds) {
            for (GameObject& child : childObjects) {
                auto& childBaseComponent = child.getComponent<GameObjectBaseComponent>();
                if (!childBaseComponent.gameObject.isValid())
                    continue;
                DrawGameObject(childBaseComponent, registry, gameObject);
            }
        }
        ImGui::TreePop();
    }

	void MainLayer::onImguiRender(Timestep timestep) {
		ImGui::DockSpaceOverViewport();

		if (ImGui::Begin(ICON_FK_THUMB_TACK "Hierarchy", 0)) {
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
                ImGui::TableSetupColumn(" " ICON_FK_EYE, ImGuiTableColumnFlags_WidthStretch, col3);
                ImGui::TableHeadersRow();

                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_CheckMark));

                entt::registry& registry = m_activeScene->getRegistry();

                childObjects.reserve(registry.alive());
                for (auto&[gameObjectID, baseComponent] : registry.storage<GameObjectBaseComponent>().each()) {
                    GameObject gameObject = baseComponent.gameObject;
                    if (!gameObject.isValid())
                        continue;
                    DrawGameObject(baseComponent, registry);
                }
                ImGui::PopStyleColor();

                ImGui::EndTable();
            }
		}
		ImGui::End();
	}
	void MainLayer::onRenderGizmo()
	{
	}
	void MainLayer::onEvent(Event& e)
	{
	}
}


