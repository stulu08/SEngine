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

		m_activeScene = createRef<Scene>();

		Resources::loadAll();
	}

	void EditorLayer::onAttach() {
		m_editorHierarchy.setScene(m_activeScene);

		Model::loadModel("assets/car.glb", m_activeScene.get());
		Model::loadModel("assets/human.glb", m_activeScene.get());
		Model::loadModel("assets/sphere.obj", m_activeScene.get());
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
	bool about = false;
	void EditorLayer::onImguiRender(Timestep timestep) {
		ImGui::DockSpaceOverViewport();
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene")) {

				}
				if (ImGui::MenuItem("Exit")) {
					Application::exit(0);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Settings")) {

				}
				ImGui::EndMenu();

			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("About"))
					about = true;

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if (about)
			if (ImGui::Begin("About - Stulu Engine")) {
				ImGui::Text("Stulu");
				ImGui::BulletText("https://github.com/stulu08/sengine");
				if (ImGui::Button("Close"))
					about = false;
				ImGui::End();

			}
		ImGui::Begin("Debug");

		if (m_runtime)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, .5f);
		else
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
		if (ImGui::Button("Play"))
			m_runtime = !m_runtime;
		ImGui::PopStyleVar();

		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Position_3d_Transform", glm::value_ptr(m_sceneCamera.getTransform().position), 3, .1f, 0, 0, "%.3f");

		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Rotation_3d_Transform", glm::value_ptr(m_sceneCamera.getTransform().rotation), 3, .1f, 0, 0, "%.3f");

		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SetCursorPosX(80.0f);
		imGui::DragScalarFloatNoLabel("Scale_3d_Transform", glm::value_ptr(m_sceneCamera.getTransform().scale), 3, .1f, 0, 0, "%.3f");
		ImGui::End();

		ImGui::Begin("Profiling");
		ImGui::Text("FPS: %.1f", 1.0f / timestep);
		ImGui::Text("Frametime: %.3f", timestep.getSeconds());
		ImGui::Text("Drawcalls: %d", ST_PROFILING_RENDERDATA_GETDRAWCALLS());
		ImGui::Text("Vertices: %d", ST_PROFILING_RENDERDATA_GETVERTICES());
		ImGui::Text("Indices: %d", ST_PROFILING_RENDERDATA_GETINDICES());
		ImGui::Text("Triangles: %d", (int)(ST_PROFILING_RENDERDATA_GETINDICES() / 3));
		ImGui::End();

		ImGui::Begin("Assets");

		const char* file[2] = { ("Stulu/assets/textures/light.png"), ("Stulu/assets/textures/Logo/engine-app-icon.png") };
		for (int i = 0; i < 2; i++) {
			ImGui::Button(file[i]);
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
				ImGui::SetDragDropPayload("ASSETS_BROWSER_MOVE_TEXTURE2D", file[i], (strlen(file[i]) + 1) * sizeof(char));
				ImGui::EndDragDropSource();
			}
		}

		ImGui::End();

		m_editorHierarchy.render();


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
		FrameBufferSpecs FBspec = m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs();
		m_sceneViewport.draw(m_sceneCamera.getCamera()->getFrameBuffer()->getTexture(),false);

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
				break;
		}
		return false;
	}
}