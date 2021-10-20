#include "EditorLayer.h"
#include <imgui/imgui.h>
#include "Editor/EditorApp.h"
#include <ImGuizmo/ImGuizmo.h>
namespace Stulu {

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_sceneCamera(0.0, 85.0f,.1f,100.0f), m_sceneViewport("Scene"), m_gameViewport("Game") {
		RenderCommand::setClearColor(glm::vec4(glm::vec3(.0f),1.0f));

		FrameBufferSpecs fspecs = FrameBufferSpecs();
		fspecs.width = Stulu::Application::get().getWindow().getWidth();
		fspecs.height = Stulu::Application::get().getWindow().getHeight();

		m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs() = fspecs;
		
		m_material = createRef<Material>(Material::fromDataStringPath("assets/Materials/red.mat"));

		Resources::loadAll();

		Previewing::init();
	}

	void EditorLayer::onAttach() {
		newScene();
	}

	void EditorLayer::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		if (m_sceneViewport.m_viewPortFocused && !m_runtime && !ImGuizmo::IsUsing())
			m_sceneCamera.onUpdate(timestep);


		if(m_runtime)
			m_activeScene->onUpdateRuntime(timestep);
		else
			m_activeScene->onUpdateEditor(timestep, m_sceneCamera);

	}
	void EditorLayer::onImguiRender(Timestep timestep) {
		ImGui::DockSpaceOverViewport();
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {

				if (ImGui::MenuItem("New Scene","Ctrl+N")) {
					newScene();
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
					OpenScene();
				}
				if (ImGui::MenuItem("Save Scene", "Ctrl+S",false, !m_currentScenePath.empty())) {
					SaveScene(m_currentScenePath);
				}
				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S")) {
					SaveScene();
				}
				if (ImGui::MenuItem("Exit","Alt+F4")) {
					Application::exit(0);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Settings")) {

				}
				ImGui::EndMenu();

			}
			ImGui::EndMainMenuBar();
		}
		ImGui::Begin("Debug");

		if (m_runtime)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, .5f);
		else
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
		if (ImGui::Button("Play"))
			m_runtime = !m_runtime;
		ImGui::PopStyleVar();

		ComponentsRender::drawComponent(GameObject::null, m_sceneCamera.getTransform());
	
		ImGui::End();

		ImGui::Begin("Profiling");
		ImGui::Text("FPS: %.1f", 1.0f / timestep);
		ImGui::Text("Frametime: %.3f", timestep.getSeconds());
#if ST_PROFILING_RENDERDATA
		if (ST_PROFILING_RENDERDATA_GETENABLE()) {
			ImGui::Text("Drawing for %d Camera(s)", ST_PROFILING_RENDERDATA_GETCAMERAS());
			ImGui::Text("Drawcalls: %d", ST_PROFILING_RENDERDATA_GETDRAWCALLS());
			ImGui::Text("Vertices: %d", ST_PROFILING_RENDERDATA_GETVERTICES());
			ImGui::Text("Indices: %d", ST_PROFILING_RENDERDATA_GETINDICES());
			ImGui::Text("Triangles: %d", (int)(ST_PROFILING_RENDERDATA_GETINDICES() / 3));
		}
#endif
		ImGui::End();

		m_editorHierarchy.render();
		m_assetBrowser.render();

		//resizing
		//scene window
		auto cam = m_activeScene->getMainCamera();
		if (cam) {
			m_gameViewport.draw(cam.getComponent<CameraComponent>().cam->getFrameBuffer()->getTexture());
			if (m_gameViewport.m_viewPortPanelWidth > 0 && m_gameViewport.m_viewPortPanelHeight > 0 && (m_activeScene->m_viewportWidth != m_gameViewport.m_viewPortPanelWidth || m_activeScene->m_viewportHeight != m_gameViewport.m_viewPortPanelHeight))
				m_activeScene->onViewportResize(m_gameViewport.m_viewPortPanelWidth, m_gameViewport.m_viewPortPanelHeight);
		}
		else {
			m_gameViewport.draw(m_sceneCamera.getCamera()->getFrameBuffer()->getTexture());
			if (m_gameViewport.m_viewPortPanelWidth > 0 && m_gameViewport.m_viewPortPanelHeight > 0 && (m_activeScene->m_viewportWidth != m_gameViewport.m_viewPortPanelWidth || m_activeScene->m_viewportHeight != m_gameViewport.m_viewPortPanelHeight))
				m_activeScene->onViewportResize(m_gameViewport.m_viewPortPanelWidth, m_gameViewport.m_viewPortPanelHeight);
		}
		//editor window
		m_sceneViewport.draw(m_sceneCamera.getCamera()->getFrameBuffer()->getTexture(),false);
		if (ImGui::BeginDragDropTarget()) {
			bool news = false;
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAG_DROP_SCENE")) {
				const char* path = (const char*)payload->Data;
				ST_INFO("Received Scene: {0}", path);
				OpenScene(path);
				news = true;
			}
			ImGui::EndDragDropTarget();
			if (news)
				return m_sceneViewport.endDraw();
		}
		FrameBufferSpecs FBspec = m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs();
		bool m_ViewportFocused = ImGui::IsWindowFocused();
		bool m_ViewportHovered = ImGui::IsWindowHovered();
		Application::get().getImGuiLayer()->blockEvents(!m_ViewportFocused && !m_ViewportHovered);

		if (m_sceneViewport.m_viewPortPanelWidth > 0 && m_sceneViewport.m_viewPortPanelHeight > 0 && (FBspec.width != m_sceneViewport.m_viewPortPanelWidth || FBspec.height != m_sceneViewport.m_viewPortPanelHeight))
			m_sceneCamera.onResize((float)m_sceneViewport.m_viewPortPanelWidth, (float)m_sceneViewport.m_viewPortPanelHeight);


		//imguizmo
		GameObject selected = m_editorHierarchy.getCurrentObject();
		if (selected && m_gizmoEditType != -1 && !m_runtime) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			const glm::mat4& cameraProjection = m_sceneCamera.getCamera()->getProjectionMatrix();
			glm::mat4 cameraView = glm::inverse(m_sceneCamera.getTransform().getTransform());

			// Gameobject transform
			TransformComponent& tc = selected.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getWorldSpaceTransform();

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_gizmoEditType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform));

			if (ImGuizmo::IsUsing()) {
				if (tc.parent) {
					glm::vec3 worldPos, worldRotation, worldScale;
					Math::decomposeTransform(transform, worldPos, worldRotation, worldScale);
					glm::mat4 parent = tc.parent.getComponent<TransformComponent>().getWorldSpaceTransform();
					const glm::vec3 parentPos = glm::vec3(parent[3]);
					parent[3] = glm::vec4(0, 0, 0, parent[3].w);
					transform = glm::translate(glm::mat4(1.0f), worldPos - parentPos) * glm::toMat4(glm::quat(worldRotation)) * glm::scale(glm::mat4(1.0f), worldScale) / parent;
				}
				glm::vec3 pos, rotation, scale;
				Math::decomposeTransform(transform, pos, rotation, scale);

				tc.position = pos;
				tc.rotation = rotation;
				tc.scale = scale;
			}
		}
		m_sceneViewport.endDraw();
	}
	void EditorLayer::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		if(!m_runtime)
			m_sceneCamera.onEvent(e);

		EventDispatcher dispacther(e);

		dispacther.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(EditorLayer::onShortCut));
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
				m_gizmoEditType = -1;
				break;
			case Keyboard::G:
				m_gizmoEditType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case Keyboard::R:
				m_gizmoEditType = ImGuizmo::OPERATION::ROTATE;
				break;
			case Keyboard::S:
				m_gizmoEditType = ImGuizmo::OPERATION::SCALE;
				if (control) {
					if (shift || m_currentScenePath.empty())
						SaveScene();
					else
						SaveScene(m_currentScenePath);
					ST_INFO("Saveing Scene");
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
		m_activeScene->onViewportResize((float)m_sceneViewport.m_viewPortPanelWidth, (float)m_sceneViewport.m_viewPortPanelHeight);
	}
	void EditorLayer::newScene() {
		m_currentScenePath = "";
		m_activeScene = createRef<Scene>();
		m_editorHierarchy.setScene(m_activeScene);
		m_activeScene->onViewportResize((float)m_sceneViewport.m_viewPortPanelWidth, (float)m_sceneViewport.m_viewPortPanelHeight);
	}
}