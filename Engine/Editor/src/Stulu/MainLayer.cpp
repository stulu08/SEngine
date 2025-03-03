#include "MainLayer.h"

#include "Style.h"
#include "Controls.h"
#include "App.h"

#include "Panels/Hierarchy.h"
#include "Panels/Profiling.h"
#include "Panels/AssetBrowser.h"
#include <Stulu/Scripting/Managed/Bindings/Core/Input.h>

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

		NewScene();
	}
	void MainLayer::onUpdate(Timestep timestep) {
		CallPanels<&Panel::Update>();

		//if (m_gameViewport.focused) {
			//Input::setCursorMode(Input::CursorMode::Disabled);
			//StuluBindings::Input::SetEnabled(false);
		//}

		if (IsRuntime()) {
			GetActiveScene()->onUpdateRuntime(timestep, false);
			//if (m_gameViewport.drawn) {
			//	m_runtimeScene->getRenderer()->GenSceneTexture(m_sceneFrameBuffer);
			//}
		}

		GetActiveScene()->onUpdateEditor(timestep, m_scenePanel->GetCamera(), m_scenePanel->IsVisible());

		DrawObjectOutlines();
	}

	void MainLayer::onImguiRender(Timestep timestep) {
		ST_PROFILING_SCOPE("Editor - ImGui");

		ImGui::DockSpaceOverViewport();

		CallPanels<&Panel::InvokeImGui>();

		DrawMenuBar();

		static bool ste = false;
		if (ImGui::Begin("Debug")) {
			ImGui::Checkbox("Style Editor", &ste);
		}
		ImGui::End();
		if(ste)
			ImGui::ShowStyleEditor();

		//Application::get().getImGuiLayer()->blockEvents(!(m_gameViewport.focused || m_sceneViewport.hovered || m_sceneViewport.focused) || Gizmo::IsUsing());
		Application::get().getImGuiLayer()->blockEvents(!(m_scenePanel->IsFocused()) || Gizmo::IsUsing());

	}
	void MainLayer::onEvent(Event& e) {
		CallPanels<&Panel::OnEvent>(e);
	}
	
	void MainLayer::StartRuntime() {
		m_runtime = true;
		m_runtimeScene = Scene::copy(m_editorScene);

		GetActiveScene()->onRuntimeStart();

		GetPanel<HierarchyPanel>().SetScene(GetActiveScene());
	}
	void MainLayer::StopRuntime() {
		GetActiveScene()->onRuntimeStop();
		
		m_runtime = false;
		m_runtimeScene = nullptr;

		GetPanel<HierarchyPanel>().SetScene(GetActiveScene());
		Time::Scale = 1.0f;
	}

	void MainLayer::onRenderGizmo() {
		CallPanels<&Panel::DrawImGuizmo>();

		const auto& selectedObjects = GetPanel<HierarchyPanel>().GetSelected();
		for (entt::entity id : selectedObjects) {
			GameObject selectedObject = { id, GetActiveScene().get()};
			if (selectedObject) {
				DrawGizmoSelected(selectedObject);
			}
		}
	}

	void MainLayer::DrawObjectOutlines() {
		auto& sceneCamera = m_scenePanel->GetCamera();
		sceneCamera.getCamera()->getFrameBuffer()->bind();
		// TODO: Draw Children, first rework transform system

		const auto& selectedObjects = GetPanel<HierarchyPanel>().GetSelected();
		for (entt::entity id : selectedObjects) {
			GameObject selected = { id, GetActiveScene().get() };
			if (selected) {
				auto& tc = selected.getComponent<TransformComponent>();

				Renderer::uploadCameraBufferData(sceneCamera.getCamera()->getProjectionMatrix(), glm::inverse(sceneCamera.getTransform().transform), glm::vec3(.0f), glm::vec3(.0f));
				RenderCommand::setDepthTesting(false);
				//draw object to stencil buffer with 0x1
				{
					RenderCommand::setStencil(StencilMode::WriteToBuffer);
					MeshFilterComponent meshFilter;
					if (selected.hasComponent<MeshRendererComponent>() && selected.saveGetComponent<MeshFilterComponent>(meshFilter)) {
						if (meshFilter.mesh.hasMesh) {
							Renderer::submit(meshFilter.mesh.mesh->getVertexArray(), Resources::GetTransparentShader(), tc.transform);
						}
					}
					RenderCommand::setStencil(StencilMode::DisableWriting);
				}
				//draw only where stencil not buffer equals 
				RenderCommand::setCullMode(CullMode::BackAndFront);
				{
					RenderCommand::setStencil(StencilMode::BeginDrawFromBuffer);
					static auto getScaleAdd = [=](const TransformComponent& tc) -> glm::vec3 {
						float scaleAdd = .02f;
						return tc.worldScale + (scaleAdd * tc.worldScale);
						};

					MeshFilterComponent meshFilter;
					if (selected.hasComponent<MeshRendererComponent>() && selected.saveGetComponent<MeshFilterComponent>(meshFilter)) {
						if (meshFilter.mesh.hasMesh) {
							Renderer::submit(meshFilter.mesh.mesh->getVertexArray(), Resources::GetOutlineShader(), Math::createMat4(tc.worldPosition, tc.worldRotation, getScaleAdd(tc)));
						}
					}
					RenderCommand::setStencil(StencilMode::EndDrawFromBuffer);
				}
			}
		}

		sceneCamera.getCamera()->getFrameBuffer()->unbind();
	}
	void MainLayer::DrawGizmoSelected(GameObject selected) {
		auto& tc = selected.getComponent<TransformComponent>();
		{
			GizmoTransformEditMode gizmoEditType = m_scenePanel->GetGizmoEditMode();

			float snapValue = .0f;
			if (Input::isKeyDown(Keyboard::LeftControl)) {
				snapValue = 0.5f;
				if (gizmoEditType == GizmoTransformEditMode::Rotate)
					snapValue = 45.0f;
			}
			if (Gizmo::TransformEdit(tc, gizmoEditType, glm::vec3(snapValue))) {
				if (IsRuntime() && GetActiveScene()->getData().enablePhsyics3D) {
					GetActiveScene()->updateTransformAndChangePhysicsPositionAndDoTheSameWithAllChilds(selected);
				}
			}
		}


		//camera view frustum
		if (selected.hasComponent<CameraComponent>()) {
			CameraComponent camera = selected.getComponent<CameraComponent>();
			float zNear = camera.settings.zNear;
			float zFar = camera.settings.zFar;
			float aspect = camera.settings.aspectRatio;
			glm::mat4 nearTransform;
			glm::mat4 farTransform;

			if (camera.mode == CameraMode::Perspective) {
				float fovHalf = glm::radians(camera.settings.fov * 0.5f);
				float nearHeight = 1.0f;
				float nearWidth = nearHeight * aspect;
				float farHeight = 2.0f * zFar * (glm::tan(fovHalf));//because tan(fov) = ankhatet / gegenkahtet irgendwas mathe 9te klasse
				float farWidth = farHeight * aspect;
				glm::vec3 nearPos = tc.worldPosition + tc.forward * zNear;
				glm::vec3 farPos = tc.worldPosition + tc.forward * zFar;

				nearTransform = Math::createMat4(nearPos, tc.worldRotation, glm::vec3(nearWidth, nearHeight, 1.0f));
				farTransform = Math::createMat4(farPos, tc.worldRotation, glm::vec3(farWidth, farHeight, 1.0f));
			}
			else {
				float zoom = camera.settings.zoom;
				float height = 1.0f * zoom;;
				float width = height * aspect;
				glm::vec3 nearPos = tc.worldPosition + tc.forward * zNear;
				glm::vec3 farPos = tc.worldPosition + tc.forward * zFar;

				nearTransform = Math::createMat4(nearPos, tc.worldRotation, glm::vec3(width, height, 1.0f));
				farTransform = Math::createMat4(farPos, tc.worldRotation, glm::vec3(width, height, 1.0f));
			}

			Gizmo::drawRect(nearTransform, COLOR_WHITE_VEC4);
			Gizmo::drawRect(farTransform, COLOR_WHITE_VEC4);
			static glm::vec4 edgesPos[4]{
				{ -0.5f, -0.5f, 0.0f, 1.0f },
				{ 0.5f, -0.5f, 0.0f, 1.0f },
				{ 0.5f,  0.5f, 0.0f, 1.0f },
				{ -0.5f,  0.5f, 0.0f, 1.0f },
			};
			for (size_t i = 0; i < 4; i++) {
				Gizmo::drawLine(nearTransform * edgesPos[i], farTransform * edgesPos[i]);
			}
		}
		if (selected.hasComponent<LightComponent>()) {
			LightComponent light = selected.getComponent<LightComponent>();

			const glm::vec3& position = tc.worldPosition;
			glm::vec3 scale = glm::vec3(1.0f);

			if (light.lightType == LightComponent::Area) {
				scale = glm::vec3(light.areaRadius * 2.0f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, 90.0f, .0f))), scale), COLOR_GREEN, .02f);
			}
			else if (light.lightType == LightComponent::Spot) {

			}

		}
		//colliders
		BoxColliderComponent boxCollider;
		if (selected.saveGetComponent<BoxColliderComponent>(boxCollider)) {
			glm::vec3 position = tc.worldPosition + boxCollider.offset;
			glm::vec3 scale = tc.worldScale * (boxCollider.size * 2.0f);
			glm::mat4 transform = Math::createMat4(position, tc.worldRotation, scale);

			Gizmo::drawOutlineCube(transform, COLOR_GREEN);
		}
		SphereColliderComponent sphereCollider;
		if (selected.saveGetComponent<SphereColliderComponent>(sphereCollider)) {
			glm::vec3 position = tc.worldPosition + sphereCollider.offset;
			glm::vec3 scale = tc.worldScale * (sphereCollider.radius * 2.0f);

			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, 90.0f, .0f))), scale), COLOR_GREEN, .02f);
		}
		RenderCommand::setWireFrame(true);
		CapsuleColliderComponent capsuleCollider;
		if (selected.saveGetComponent<CapsuleColliderComponent>(capsuleCollider)) {
			glm::vec3 position = tc.worldPosition + capsuleCollider.offset;
			glm::vec3 scale;
			if (capsuleCollider.horizontal)
				scale = tc.worldScale * (glm::vec3(capsuleCollider.radius, capsuleCollider.height / 2.0f, capsuleCollider.radius));
			else
				scale = tc.worldScale * (glm::vec3(capsuleCollider.height / 2.0f, capsuleCollider.radius, capsuleCollider.radius));

			Renderer::submit(Stulu::Resources::getCapsuleMeshAsset().mesh->getVertexArray(),
				Resources::GetHighliteShader(),
				Math::createMat4(position, tc.worldRotation, scale));
		}
		MeshColliderComponent meshCollider;
		if (selected.saveGetComponent<MeshColliderComponent>(meshCollider)) {
			glm::vec3 position = tc.worldPosition;
			glm::vec3 scale = tc.worldScale;

			if (meshCollider.mesh.hasMesh) {
				if (meshCollider.convex && meshCollider.convexMesh)
					Renderer::submit(meshCollider.convexMesh->getVertexArray(),
						Resources::GetHighliteShader(),
						Math::createMat4(position, tc.worldRotation, scale));
				else
					Renderer::submit(meshCollider.mesh.mesh->getVertexArray(),
						Resources::GetHighliteShader(),
						Math::createMat4(position, tc.worldRotation, scale));
			}
		}
		RenderCommand::setWireFrame(false);
	}

	void MainLayer::DrawMenuBar() {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New Scene")) {
					NewScene();
				}
				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) {
					OpenScene();
				}
				if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, !m_currentScenePath.empty())) {
					SaveScene(m_currentScenePath, false);
				}
				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S")) {
					SaveScene(m_currentScenePath, true);
				}
				if (ImGui::MenuItem("Exit", "Alt+F4")) {
					Application::get().exit(0);
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings")) {
				if (ImGui::MenuItem("VSync", "", App::get().getWindow().isVSync())) {
					App::get().getWindow().setVSync(!App::get().getWindow().isVSync());
				}
				if (PhysX::isPVDRunning()) {
					if (ImGui::MenuItem("Stop PhysX Visual Debugger")) {
						PhysX::stopPVD();
					}
				}
				else {
					if (ImGui::MenuItem("Start PhysX Visual Debugger")) {
						if (!PhysX::started()) {
							PhysX::startUp();
						}
						PhysX::startPVD();
					}
				}


				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {
				for (auto& [hash, panel] : m_panels) {
					if (ImGui::MenuItem(panel->GetID().c_str(), 0, panel->IsOpen())) {
						panel->SetOpen(!panel->IsOpen());
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("About")) {
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	
	void MainLayer::SaveScene(const std::string& path, bool browse) {
		if (browse) {
			std::string recommendPath = path.empty() ? App::get().GetProject().GetAssetPath() : path;
			std::string scenePath = Platform::saveFile("Scene File (*.scene)\0 *.scene\0All Files (*.*)\0*.*\0", recommendPath.c_str());
			if (std::filesystem::exists(scenePath))
				SaveScene(scenePath, false);

			return;
		}

		if (std::filesystem::exists(path)) {
			m_currentScenePath = path;
			SceneSerializer ss(m_editorScene);
			ss.serialze(path);
			ST_TRACE("Saved Scene");
			return;
		}
	}
	void MainLayer::OpenScene(const std::string& path) {
		if (IsRuntime() || !std::filesystem::exists(path))
			return;

		m_currentScenePath = path;
		Ref<Scene> newScene = createRef<Scene>();
		SceneSerializer ss(newScene);
		if (ss.deSerialze(path)) {
			OpenScene(newScene);
		}
	}
	void MainLayer::OpenScene() {
		std::string path = Platform::openFile("Scene Files (*.scene)\0 *.scene\0All Files (*.*)\0*.*\0", App::get().GetProject().GetAssetPath().c_str());
		if (std::filesystem::exists(path))
			OpenScene(path);
	}
	void MainLayer::OpenScene(Ref<Scene> scene) {
		if (IsRuntime())
			return;

		m_editorScene = scene;
		GetActiveScene()->onViewportResize(m_scenePanel->GetWidth(), m_scenePanel->GetHeight());
		GetPanel<HierarchyPanel>().SetScene(GetActiveScene());
		ST_TRACE("New Scene loaded");
	}
}


