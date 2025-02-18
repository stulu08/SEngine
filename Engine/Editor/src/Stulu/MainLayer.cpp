#include "MainLayer.h"

#include "Style.h"
#include "Controls.h"
#include "App.h"

#include "Panels/Hierarchy.h"
#include "Panels/Profiling.h"

using namespace Stulu;

namespace Editor {
	MainLayer::MainLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f, .001f, 1000.0f, 1) {
		
		AddPanel<HierarchyPanel>();
		AddPanel<ProfilingPanel>();
	}
	MainLayer::~MainLayer()
	{}

	void MainLayer::onAttach() {
		ImGui::SetCurrentContext(Application::get().getImGuiLayer()->getContext());
		Style::LoadAll();

		// load debug Startup scene
        const std::string path = Editor::App::get().GetProject().GetAssetPath() + "/scenes/Sponza.scene";
        m_activeScene = createRef<Scene>();
        SceneSerializer ss(m_activeScene);
        if (ss.deSerialze(path)) {
            m_activeScene->onViewportResize(100, 100);
            ST_TRACE("Opened Scene {0}", path);
        }

		GetPanel<HierarchyPanel>().SetScene(m_activeScene);
	}
	void MainLayer::onUpdate(Timestep timestep) {
	}

	void MainLayer::onImguiRender(Timestep timestep) {
		ImGui::DockSpaceOverViewport();

		CallPanels<&Panel::InvokeImGui>();


		static bool ste = false;
		if (ImGui::Begin("Debug")) {
			ImGui::Checkbox("Style Editor", &ste);
		}
		ImGui::End();
		
		if(ste)
			ImGui::ShowStyleEditor();
	}
	void MainLayer::onRenderGizmo() {
		CallPanels<&Panel::DrawImGuizmo>();

	}
	void MainLayer::onEvent(Event& e)
	{
	}
}


