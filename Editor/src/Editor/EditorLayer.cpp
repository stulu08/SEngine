#include "EditorLayer.h"
#include "Editor/EditorApp.h"
#include "Editor/DiscordRPC.h"
#include "Editor/Panel/StyleEditor.h"
#include <ImGuizmo/ImGuizmo.h>

namespace Stulu {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f,.1f,100.0f) {
		StyleEditor::init();

		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f),1.0f));

		FrameBufferSpecs fspecs = FrameBufferSpecs();
		fspecs.width = Stulu::Application::get().getWindow().getWidth();
		fspecs.height = Stulu::Application::get().getWindow().getHeight();

		m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs() = fspecs;
		

		EditorResources::loadAll();
		Previewing::init();
	}

	void EditorLayer::onAttach() {
		newScene();
	}

	void EditorLayer::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		if (!ImGuizmo::IsUsing()) {
			if(m_sceneViewport.focused)
				m_sceneCamera.updateMove(timestep);
			m_sceneCamera.onUpdate(timestep);
		}


		if(s_runtime)
			m_activeScene->onUpdateRuntime(timestep);

		m_activeScene->onUpdateEditor(timestep, m_sceneCamera);

	}
	void EditorLayer::onImguiRender(Timestep timestep) {
		ImGui::DockSpaceOverViewport();

		{

			ImGui::Begin("Profiling");
			ImGui::Text("FPS: %.1f", 1.0f / timestep);
			ImGui::Text("Frametime: %.3f", Time::deltaTime.getSeconds());
			if (ImGui::TreeNodeEx("Memory Usage", ImGuiTreeNodeFlags_DefaultOpen)) {
				MemoryUsageInfo info = Platform::getMemoryUsage();
				double sqrt = 1048576;
				ImGui::Text("Virtual memory usage: %smb", 
					std::to_string(static_cast<double>(info.virtualUsedByProcess) / sqrt).c_str());
				ImGui::Text("Physical memory usage: %smb",
					std::to_string(static_cast<double>(info.physicalUsedByProcess) / sqrt).c_str());
				ImGui::TreePop();
			}
#if ST_PROFILING_RENDERDATA
			if (ST_PROFILING_RENDERDATA_GETENABLE() && ImGui::TreeNodeEx("Render Data", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Text("Runtime time: %.1f", Time::time.getSeconds());
				ImGui::Text("Drawing for %d Camera(s)", ST_PROFILING_RENDERDATA_GETCAMERAS());
				ImGui::Text("Drawcalls: %d", ST_PROFILING_RENDERDATA_GETDRAWCALLS());
				ImGui::Text("Vertices: %d", ST_PROFILING_RENDERDATA_GETVERTICES());
				ImGui::Text("Indices: %d", ST_PROFILING_RENDERDATA_GETINDICES());
				ImGui::Text("Triangles: %d", (int)(ST_PROFILING_RENDERDATA_GETINDICES() / 3));
			}
#endif
			ImGui::End();
		}

		Application::get().getImGuiLayer()->blockEvents(!m_sceneViewport.hovered && !m_sceneViewport.focused);


		drawMenuBar();

		if (m_showStyleEditor) {
			StyleEditor::drawStyleEditor(&ImGui::GetStyle(), &m_showStyleEditor);
		}
		if (m_showHierarchy) {
			m_editorHierarchy.render(&m_showHierarchy);
		}
		if (m_showInspector) {
			if (!m_inspectorPanel.render(m_editorHierarchy.getCurrentObject(), &m_showInspector)) {
				m_editorHierarchy.setSelectedGameObject(GameObject::null);
			}
		}
		if (m_showAssetBrowser) {
			m_assetBrowser.render(&m_showAssetBrowser);
		}
		if (m_showGameViewport) {
			auto cam = m_activeScene->getMainCamera();
			if (cam) {
				m_gameViewport.draw(cam.getComponent<CameraComponent>().cam, &m_showGameViewport);
				if (m_gameViewport.width > 0 && m_gameViewport.height > 0 && (m_activeScene->m_viewportWidth != m_gameViewport.width || m_activeScene->m_viewportHeight != m_gameViewport.height))
					m_activeScene->onViewportResize(m_gameViewport.width, m_gameViewport.height);
			}
			else {
				m_gameViewport.draw(nullptr, &m_showGameViewport);
			}
		}
		if (m_showSceneViewport) {
			m_sceneViewport.draw(m_sceneCamera, m_editorHierarchy.getCurrentObject(), &m_showSceneViewport);
		}

	}
	void EditorLayer::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();

		m_sceneCamera.onEvent(e);

		EventDispatcher dispacther(e);

		dispacther.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(EditorLayer::onShortCut));
	}
	void EditorLayer::drawMenuBar() {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
					newScene();
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
					OpenScene();
				}
				if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, !m_currentScenePath.empty())) {
					SaveScene(m_currentScenePath);
				}
				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S")) {
					SaveScene();
				}
				if (ImGui::MenuItem("Exit", "Alt+F4")) {
					Application::exit(0);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				if (ImGui::MenuItem("Style Editor", (const char*)0, m_showStyleEditor)) {
					m_showStyleEditor = !m_showStyleEditor;
				}
				if (ImGui::MenuItem("Game Viewport", (const char*)0, m_showGameViewport)) {
					m_showGameViewport = !m_showGameViewport;
				}
				if (ImGui::MenuItem("Scene Viewport", (const char*)0, m_showSceneViewport)) {
					m_showSceneViewport = !m_showSceneViewport;
				}
				if (ImGui::MenuItem("Scene Hierachy", (const char*)0, m_showHierarchy)) {
					m_showHierarchy = !m_showHierarchy;
				}
				if (ImGui::MenuItem("Inspector", (const char*)0, m_showInspector)) {
					m_showInspector = !m_showInspector;
				}
				if (ImGui::MenuItem("Asset Browser", (const char*)0, m_showAssetBrowser)) {
					m_showAssetBrowser = !m_showAssetBrowser;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		//toolbar
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
		float size = ImGui::GetWindowHeight() - 4.0f;
		ImVec4 icoColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size));
		Ref<Texture> tex = s_runtime ? EditorResources::getStopTexture() : EditorResources::getPlayTexture();
		if (ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)tex->getRendererID()), {size, size}, {0, 1}, {1, 0}, 0, {0,0,0,0}, icoColor)) {
			if (s_runtime) {
				m_activeScene->onRuntimeStop();
				s_runtime = false;
			}
			else {
				m_activeScene->onRuntimeStart();
				s_runtime = true;
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}
	bool EditorLayer::onShortCut(KeyDownEvent& e) {
		ST_PROFILING_FUNCTION();
		if (e.getRepeatCount() > 0)
			return false;

		bool control = Input::isKeyDown(Keyboard::LeftControl) || Input::isKeyDown(Keyboard::RightControl);
		bool shift = Input::isKeyDown(Keyboard::LeftShift) || Input::isKeyDown(Keyboard::RightShift);

		switch (e.getKeyCode())
		{

			case Keyboard::Q:
				m_sceneViewport.gizmoEditType = -1;
				break;
			case Keyboard::G:
				m_sceneViewport.gizmoEditType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Keyboard::R:
				m_sceneViewport.gizmoEditType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Keyboard::S:
				m_sceneViewport.gizmoEditType = ImGuizmo::OPERATION::SCALE;
				if (control) {
					if (shift || m_currentScenePath.empty())
						SaveScene();
					else
						SaveScene(m_currentScenePath);
					ST_INFO("Saving Scene");
				}
				break;
			case Keyboard::O:
				if (control) {
					OpenScene();
					ST_INFO("Opening Scene");
				}
				break;
			case Keyboard::N:
				if (control) {
					newScene();
					ST_INFO("New Scene");
				}
				break;
		}
		return false;
	}
	void EditorLayer::SaveScene() {
		std::string path = Platform::saveFile("All\0 * .*\0Scene Files\0 * .scene\0");
		if (!path.empty())
			SaveScene(path);
	}
	void EditorLayer::OpenScene() {
		std::string path = Platform::openFile("All\0 * .*\0Scene Files\0 * .scene\0");
		if (!path.empty())
			OpenScene(path);
	}
	void EditorLayer::SaveScene(const std::string& path) {
		if (path.empty()) {
			SaveScene();
		}
		m_currentScenePath = path;
		SceneSerializer ss(m_activeScene);
		ss.serialze(path);
		DiscordRPC::setDetails("Editing " + path.substr(path.find_last_of("/\\")+1, path.npos));
		ST_INFO("Saved Scene {0}", path);
	}
	void EditorLayer::OpenScene(const std::string& path) {
		if (path.empty()) {
			OpenScene();
		}
		m_currentScenePath = path;
		Ref<Scene> nScene = createRef<Scene>();
		SceneSerializer ss(nScene);
		ss.deSerialze(path);
		m_activeScene = nScene;
		m_editorHierarchy.setScene(m_activeScene);
		m_activeScene->onViewportResize(m_sceneViewport.width, m_sceneViewport.height);
		ST_INFO("Opened Scene {0}", path);
		DiscordRPC::setDetails("Editing " + path.substr(path.find_last_of("/\\")+1, path.npos));
	}
	void EditorLayer::newScene() {
		m_currentScenePath = "";
		m_activeScene = createRef<Scene>();
		m_editorHierarchy.setScene(m_activeScene);
		m_activeScene->onViewportResize(m_sceneViewport.width, m_sceneViewport.height);
		DiscordRPC::setDetails("Editing a scene");
		ST_INFO("New Scene loaded");
	}
}