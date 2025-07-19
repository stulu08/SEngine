#include "MainLayer.h"

#include "Style.h"
#include "Controls.h"
#include "App.h"

#include "Panels/Hierarchy.h"
#include "Panels/Profiling.h"
#include "Panels/AssetBrowser.h"
#include "Panels/Inspector.h"
#include "Panels/Settings.h"
#include "Panels/Shortcut.h"

#include <Stulu/Scripting/Managed/Bindings/Core/Input.h>
#include <Stulu/Physics/Components/Collider.h>
#include <Stulu/Resources/Assets/MeshAsset.h>
using namespace Stulu;

namespace Editor {
	MainLayer::MainLayer()
		: Layer("EditorLayer"), m_preview() {
		
		LoadShotcuts();
	}
	MainLayer::~MainLayer() {
		for (auto& [hash, panel] : m_panels) {
			if(panel)
				delete panel;
		}
	}

	static Mono::Method debugMethod = nullptr;

	void MainLayer::onAttach() {
		{
			AddPanel<LogPanel>();
			auto sink = createRef<MemoryLogSink>();
			sink->set_formatter(std::make_unique<spdlog::pattern_formatter>("%v"));
			Log::AddSink(sink);
			m_logPanel = &GetPanel<LogPanel>();
		}



		AddPanel<HierarchyPanel>();
		AddPanel<ProfilingPanel>();
		AddPanel<AssetBrowser>(App::get().GetProject().GetAssetPath());
		AddPanel<ScenePanel>();
		AddPanel<GamePanel>();
		AddPanel<InspectorPanel>();
		AddPanel<SettingsPanel>();

		m_scenePanel = &GetPanel<ScenePanel>();
		m_gamePanel = &GetPanel<GamePanel>();

		ImGui::SetCurrentContext(Application::get().getImGuiLayer()->getContext());
		Style::LoadAll();
		
		debugMethod = App::get().getAssemblyManager()->getScriptCoreAssembly()->CreateMethod("Editor", "Editor", "DebugTest");
	}
	void MainLayer::onLoadFinish() {
		App::get().GetPrefs().Section("Editor");
		std::string scenePath = App::get().GetPrefs().Get("Scene", "");
		if (std::filesystem::exists(scenePath)) {
			OpenScene(scenePath);
		}
		else {
			NewScene();
		}

		std::sort(m_shortcuts.begin(), m_shortcuts.end(), std::greater<Shortcut>());
		AddPanel<ShortcutPanel>();
	}


	void MainLayer::onUpdate(Timestep timestep) {
		ST_PROFILING_SCOPE("Editor - Update");

		CallPanels<&Panel::Update>();

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

			static glm::vec3 t(1.f);
			static Stulu::UUID defaultMaterial = Stulu::Resources::UUIDDefaultMaterial;
			static Stulu::UUID reflectiveMaterial = Stulu::Resources::UUIDReflectiveMaterial;
			static Stulu::UUID editMaterial = Stulu::Resources::UUIDDefaultMaterial;
			Controls::Material("Default Material", defaultMaterial);
			Controls::Material("Reflective Material", reflectiveMaterial);
			Controls::Material("Edit Material", editMaterial);
			if (editMaterial) {
				auto material = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(editMaterial);

				auto tMode = material->GetTransparencyMode();
				if (Controls::Combo("Transparency Mode", tMode)) {
					material->SetTransparencyMode(tMode);
				}
				
				for (auto& metaProp : material->GetProperities()) {
					if (metaProp->GetType() == MaterialPropertyType::Sampler2D) {
						auto prop = material->GetProperityAs<MaterialSampler2DProperty>(metaProp->GetName());
						auto val = prop->GetValue();
						if (Controls::Default(prop->GetName(), val))
							prop->SetValue(val);
					}
					if (metaProp->GetType() == MaterialPropertyType::Color) {
						auto prop = material->GetProperityAs<MaterialColorProperty>(metaProp->GetName());
						auto val = prop->GetValue();
						if (Controls::Color(prop->GetName(), val, prop->IsHDR()))
							prop->SetValue(val);
					}
					if (metaProp->GetType() == MaterialPropertyType::Float2) {
						auto prop = material->GetProperityAs<MaterialFloat2Property>(metaProp->GetName());
						auto val = prop->GetValue();
						if (Controls::Default(prop->GetName(), val))
							prop->SetValue(val);
					}
					if (metaProp->GetType() == MaterialPropertyType::Float) {
						auto prop = material->GetProperityAs<MaterialFloatProperty>(metaProp->GetName());
						auto val = prop->GetValue();
						if (prop->HasLimits()) {
							if (Controls::Slider::Float(prop->GetName(), val, prop->GetMin(), prop->GetMax()))
								prop->SetValue(val);
						}
						else {
							if (Controls::Default(prop->GetName(), val))
								prop->SetValue(val);
						}
						
					}
					metaProp->ApplyValue(*material);
				}
			}


			App::get().getAssemblyManager()->getScriptCoreAssembly()->InvokeMethod(debugMethod, NULL, NULL);
		}
		
		ImGui::End();
		if(ste)
			ImGui::ShowStyleEditor();

		RenderImGuiNotifications();

		// compiler will do the optimization ahh code
		bool blockEvents = true;
		if (m_gamePanel->IsFocused())
			blockEvents = false;
		if (m_scenePanel->IsFocused())
			blockEvents = false;
		if (Gizmo::WasHovered())
			blockEvents = true;

		Application::get().getImGuiLayer()->blockEvents(blockEvents);

		if (!m_gamePanel->IsFocused())
			CheckShortcuts();

	}
	void MainLayer::onEvent(Event& e) {
		ST_PROFILING_SCOPE("Editor - Event");

		EventDispatcher dispatcher(e);
		// shortcuts should be always checked, when imgui wants to capture the keyboard events are stopped resulting in the layer not receiving the keyboard event
		//dispatcher.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(MainLayer::CheckShortcuts));

		CallPanels<&Panel::OnEvent>(e);

		if (e.getEventType() == EventType::WindowClose) {
			StopRuntime();
		}
		GetActiveScene()->onEvent(e);

	}

	void MainLayer::onRenderGizmo() {
		ST_PROFILING_SCOPE("Editor - Gizmos");

		const auto& selectedObjects = GetPanel<HierarchyPanel>().GetSelected();
		for (entt::entity id : selectedObjects) {
			GameObject selectedObject = { id, GetActiveScene().get() };
			if (selectedObject.IsValid()) {
				DrawGizmoSelected(selectedObject);
			}
		}

		GizmoTransformEditMode gizmoEditType = m_scenePanel->GetGizmoEditMode();
		float snapValue = Input::isKeyDown(Keyboard::LeftControl)
			? (gizmoEditType == GizmoTransformEditMode::Rotate ? 45.0f : 0.5f)
			: 0.0f;
		Gizmo::TransformEdit(selectedObjects, GetActiveScene().get(), gizmoEditType, glm::vec3(snapValue));



		CallPanels<&Panel::DrawImGuizmo>();
		GetActiveScene()->getCaller()->onDrawGizmos();
	}

	void MainLayer::DrawObjectStencilOutline(entt::entity objID) {
		GameObject object = { objID, GetActiveScene().get() };
		TransformComponent& transform = object.getComponent<TransformComponent>();


		if (object.hasComponent<MeshRendererComponent>() && object.hasComponent<MeshFilterComponent>()) {
			MeshRendererComponent& mesRenderer = object.getComponent<MeshRendererComponent>();
			mesRenderer.StencilValue = 0xFF;

			MeshFilterComponent& meshFilter = object.getComponent<MeshFilterComponent>();
			if (meshFilter.GetMesh().IsValid()) {
				Resources::GetOutlineShader()->bind();
				Renderer::UploadModelData(transform.GetWorldTransform() * glm::scale(glm::mat4(1.0f), glm::vec3(1.05f)));
				RenderCommand::drawIndexed(meshFilter.GetMesh()->GetVertexArray());
			}
		}
		
		if (transform.HasChildren()) {
			for (entt::entity childID : transform.GetChildren()) {
				DrawObjectStencilOutline(childID);
			}
		}
	}

	void MainLayer::DrawObjectOutlines() {
		auto& sceneCamera = m_scenePanel->GetCamera();
		sceneCamera.getCamera().getResultFrameBuffer()->bind();

		Renderer::uploadCameraBufferData(sceneCamera.getCamera().getProjectionMatrix(), glm::inverse(sceneCamera.getTransform().GetWorldTransform()), glm::vec3(.0f), glm::vec3(.0f));
		const auto& selectedObjects = GetPanel<HierarchyPanel>().GetSelected();

		RenderCommand::SetStencilValue(0x00);
		RenderCommand::StencilNotEqual(0xFF);
		RenderCommand::setDepthTesting(false);
		for (entt::entity id : selectedObjects) {
			if (GetActiveScene()->IsValid(id)) {
				DrawObjectStencilOutline(id);
			}
		}
		RenderCommand::setDefault();


		sceneCamera.getCamera().getResultFrameBuffer()->unbind();
	}
	void MainLayer::DrawGizmoSelected(GameObject selected) {
		auto& tc = selected.getComponent<TransformComponent>();

		//camera view frustum
		if (selected.hasComponent<CameraComponent>()) {
			const auto& camera = selected.getComponent<CameraComponent>();
			float zNear = camera.GetNear();
			float zFar = camera.GetFar();
			float aspect = camera.GetAspect();
			glm::mat4 nearTransform;
			glm::mat4 farTransform;

			if (camera.GetMode() == CameraMode::Perspective) {
				float fovHalf = glm::radians(camera.GetFov() * 0.5f);
				float nearHeight = 1.0f;
				float nearWidth = nearHeight * aspect;
				float farHeight = 2.0f * zFar * (glm::tan(fovHalf));//because tan(fov) = ankhatet / gegenkahtet irgendwas mathe 9te klasse
				float farWidth = farHeight * aspect;
				glm::vec3 nearPos = tc.GetWorldPosition() + tc.GetForward() * zNear;
				glm::vec3 farPos = tc.GetWorldPosition() + tc.GetForward() * zFar;

				nearTransform = Math::createMat4(nearPos, tc.GetWorldRotation(), glm::vec3(nearWidth, nearHeight, 1.0f));
				farTransform = Math::createMat4(farPos, tc.GetWorldRotation(), glm::vec3(farWidth, farHeight, 1.0f));
			}
			else {
				float zoom = camera.GetZoom();
				float height = 1.0f * zoom;;
				float width = height * aspect;
				glm::vec3 nearPos = tc.GetWorldPosition() + tc.GetForward() * zNear;
				glm::vec3 farPos = tc.GetWorldPosition() + tc.GetForward() * zFar;

				nearTransform = Math::createMat4(nearPos, tc.GetWorldRotation(), glm::vec3(width, height, 1.0f));
				farTransform = Math::createMat4(farPos, tc.GetWorldRotation(), glm::vec3(width, height, 1.0f));
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

			const glm::vec3& position = tc.GetWorldPosition();
			glm::vec3 scale = glm::vec3(1.0f);

			if (light.Type == LightType::Point) {
				scale = glm::vec3(light.Point.Radius * 2.0f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
				Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, 90.0f, .0f))), scale), COLOR_GREEN, .02f);
			}

		}
		//colliders
		if (selected.hasComponent<BoxColliderComponent>()) {
			BoxColliderComponent& boxCollider = selected.getComponent<BoxColliderComponent>();

			glm::vec3 position = tc.GetWorldPosition() + boxCollider.GetOffset();
			glm::vec3 scale = tc.GetWorldScale() * (boxCollider.GetSize() * 2.0f);
			glm::mat4 transform = Math::createMat4(position, tc.GetWorldRotation(), scale);

			Gizmo::drawOutlineCube(transform, COLOR_GREEN);
		}
		if (selected.hasComponent<SphereColliderComponent>()) {
			SphereColliderComponent& sphereCollider = selected.getComponent<SphereColliderComponent>();
			glm::vec3 position = tc.GetWorldPosition() + sphereCollider.GetOffset();
			glm::vec3 scale = tc.GetWorldScale() * (sphereCollider.GetRadius() * 2.0f);

			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(90.0f, .0f, .0f))), scale), COLOR_GREEN, .02f);
			Renderer2D::drawCircle(Math::createMat4(position, glm::quat(glm::radians(glm::vec3(.0f, 90.0f, .0f))), scale), COLOR_GREEN, .02f);
		}
		RenderCommand::setWireFrame(true);
		if (selected.hasComponent<CapsuleColliderComponent>()) {
			CapsuleColliderComponent& capsuleCollider = selected.getComponent<CapsuleColliderComponent>();
			const glm::vec3 position = tc.GetWorldPosition() + capsuleCollider.GetOffset();
			glm::quat rotation = tc.GetWorldRotation();
			const float radius = capsuleCollider.GetRadius() * 2.0f;
			const float halfHeight = (capsuleCollider.GetHeight() * 0.5f);

			glm::vec3 capsuleScale;
			if (capsuleCollider.GetVertical()) {
				capsuleScale = tc.GetWorldScale() * glm::vec3(radius, radius, halfHeight);
				rotation *= glm::quat(glm::vec3(glm::radians(90.0f), 0, 0));
			}
			else {
				capsuleScale = tc.GetWorldScale() * glm::vec3(halfHeight, radius, radius);
			}
			Resources::GetHighliteShader()->bind();
			Renderer::UploadModelData(Math::createMat4(position, rotation, capsuleScale));
			RenderCommand::drawIndexed(Stulu::Resources::CapsuleMesh()->GetVertexArray());
		}
		if (selected.hasComponent<MeshColliderComponent>()) {
			MeshColliderComponent& meshCollider = selected.getComponent<MeshColliderComponent>();
			glm::vec3 position = tc.GetWorldPosition();
			glm::vec3 scale = tc.GetWorldScale();

			if (meshCollider.GetMesh().IsValid()) {
				Resources::GetHighliteShader()->bind();
				Renderer::UploadModelData(Math::createMat4(position, tc.GetWorldRotation(), scale));
				RenderCommand::drawIndexed(meshCollider.GetMesh()->GetVertexArray());

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
					const bool value = !App::get().getWindow().isVSync();
					App::get().getWindow().setVSync(value);
					App::get().GetPrefs().Set("Editor", "Vsync", value);
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
	void MainLayer::StartRuntime() {
		m_runtime = true;
		SetScene(Scene::copy(m_editorScene));

		GetActiveScene()->onRuntimeStart();
		ImGui::SetWindowFocus(m_gamePanel->GetID().c_str());

		if (m_logPanel->IsClearOnPlay())
			m_logPanel->Clear();
	}
	void MainLayer::StopRuntime() {
		GetActiveScene()->onRuntimeStop();

		m_runtime = false;
		SetScene(m_editorScene);

		Time::Scale = 1.0f;
		ImGui::SetWindowFocus(m_scenePanel->GetID().c_str());
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
			SceneSerializer ss(m_editorScene.get());
			ss.Serialze(path);
			ST_TRACE("Saved Scene");
			ImGui::InsertNotification({ ImGuiToastType::Info, "Scene Saved", path.c_str() });
		}
		else {
			ImGui::InsertNotification({ ImGuiToastType::Error, "Error Saving Scene" });
		}
	}
	void MainLayer::OpenScene(const std::string& path) {
		if (IsRuntime() || !std::filesystem::exists(path))
			return;

		m_currentScenePath = path;
		Ref<Scene> newScene = createRef<Scene>();
		SceneSerializer ss(newScene.get());
		if (ss.Deserialze(path)) {
			OpenScene(newScene);
			App::get().GetPrefs().Set("Editor", "Scene", path);
			ImGui::InsertNotification({ ImGuiToastType::Info, "Loaded Scene: %s", path.c_str()});
		}
		else {
			ImGui::InsertNotification({ ImGuiToastType::Error, "Error loading Scene" });
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
		SetScene(scene);
		ST_TRACE("New Scene loaded");
	}

	void MainLayer::SetScene(Ref<Scene> scene) {
		if (IsRuntime()) {
			m_runtimeScene = scene;
		}
		else {
			m_editorScene = scene;
			m_runtimeScene = nullptr;
		}

		StuluBindings::SetCurrentRegistry(GetActiveScene().get());

		if (IsRuntime()) {
			GetActiveScene()->onViewportResize(m_gamePanel->GetWidth(), m_gamePanel->GetHeight());
		}
		else {
			GetActiveScene()->onViewportResize(m_scenePanel->GetWidth(), m_scenePanel->GetHeight());
		}

		GetPanel<HierarchyPanel>().SetScene(GetActiveScene());
	}
	void MainLayer::LoadShotcuts() {

		// Open Scene Ctrl+0
		m_shortcuts.push_back(Shortcut("Open Scene", [&]() { OpenScene(); return true; }, Keyboard::O, false, true));
		// Save Scene Ctrl+S
		m_shortcuts.push_back(Shortcut("Save Scene", [&]() { SaveScene(m_currentScenePath, false); return true; }, Keyboard::S, false, true));
		// Save Scene As Ctrl+Shift+S
		m_shortcuts.push_back(Shortcut("Save Scene as", [&]() { SaveScene(m_currentScenePath, true); return true; }, Keyboard::S, true, true));
		// Switch Runtime Mode Ctrl+Alt+P
		m_shortcuts.push_back(Shortcut("Switch Runtime Mode", ([&]() {
			if (this->IsRuntime())
				StopRuntime();
			else
				StartRuntime();
			return true;
		}), Keyboard::P, false, true, true));

	}
	bool MainLayer::CheckShortcuts() {
		const bool ctrl = Input::isKeyDown(Keyboard::LeftControl) || Input::isKeyDown(Keyboard::RightControl);
		const bool shift = Input::isKeyDown(Keyboard::LeftShift) || Input::isKeyDown(Keyboard::RightShift);
		const bool alt = Input::isKeyDown(Keyboard::LeftAlt) || Input::isKeyDown(Keyboard::RightAlt);


		for (const auto& shortcut : m_shortcuts) {
			if (shortcut.UseControl() != ctrl) continue;
			if (shortcut.UseShift() != shift) continue;
			if (shortcut.UseAlt() != alt) continue;


			if (Input::getKeyDown(shortcut.GetKeyCode())) {
				if (shortcut.OnPress())
					return true;
			}
		}
		return false;
	}
}


