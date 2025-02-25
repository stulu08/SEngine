#include "MainLayer.h"

#include "Style.h"
#include "Controls.h"
#include "App.h"

#include "Panels/Hierarchy.h"
#include "Panels/Profiling.h"
#include "Panels/AssetBrowser.h"

using namespace Stulu;

namespace Editor {
	MainLayer::MainLayer()
		: Layer("EditorLayer"), m_preview() {
		
		AddPanel<HierarchyPanel>();
		AddPanel<ProfilingPanel>();
		AddPanel<AssetBrowser>(App::get().GetProject().GetAssetPath());

		AddPanel<ScenePanel>();
		m_scenePanel = &GetPanel<ScenePanel>();
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
		if (!Gizmo::IsUsing()) {
			Input::setEnabled(m_scenePanel->IsFocused());
			m_scenePanel->GetCamera().updateMove(timestep);
			m_scenePanel->GetCamera().onUpdate(timestep);
			m_activeScene->updateTransform(m_scenePanel->GetCamera().getTransform());
			Input::setEnabled(true);
		}

		m_activeScene->onUpdateEditor(timestep, m_scenePanel->GetCamera(), m_scenePanel->IsVisible());

	}

	void MainLayer::onImguiRender(Timestep timestep) {
		ST_PROFILING_SCOPE("Editor - ImGui");

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
		if (m_scenePanel->BeginGizmo()) {
			CallPanels<&Panel::DrawImGuizmo>();
		}
		m_scenePanel->EndGizmo();

	}
	void MainLayer::onEvent(Event& e) {
		CallPanels<&Panel::OnEvent>(e);
	}
}


