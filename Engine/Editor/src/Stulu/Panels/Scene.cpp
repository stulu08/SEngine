#include "Scene.h"

#include <imgui_internal.h>

#include <Stulu/ImGui/Gizmo.h>
#include <Stulu/Physics/PhysicsScene.h>

#include "Stulu/App.h"
#include "Stulu/Controls.h"
#include "Stulu/Style.h"
#include "Stulu/MainLayer.h"
#include "Hierarchy.h"


using namespace Stulu;

constexpr uint32_t NoneFlag = (uint32_t)GizmoTransformEditMode::None;
constexpr uint32_t UniversalFlag = (uint32_t)GizmoTransformEditMode::Universal;

constexpr uint32_t TranslateFlag = (uint32_t)GizmoTransformEditMode::Translate;
constexpr uint32_t TranslateFlag_X = (uint32_t)GizmoTransformEditMode::Translate_X;
constexpr uint32_t TranslateFlag_Y = (uint32_t)GizmoTransformEditMode::Translate_Y;
constexpr uint32_t TranslateFlag_Z = (uint32_t)GizmoTransformEditMode::Translate_Z;

constexpr uint32_t RotateFlag = (uint32_t)GizmoTransformEditMode::Rotate;
constexpr uint32_t RotateFlag_X = (uint32_t)GizmoTransformEditMode::Rotate_X;
constexpr uint32_t RotateFlag_Y = (uint32_t)GizmoTransformEditMode::Rotate_Y;
constexpr uint32_t RotateFlag_Z = (uint32_t)GizmoTransformEditMode::Rotate_Z;

constexpr uint32_t ScaleFlag = (uint32_t)GizmoTransformEditMode::Scale;
constexpr uint32_t ScaleFlag_X = (uint32_t)GizmoTransformEditMode::Scale_X;
constexpr uint32_t ScaleFlag_Y = (uint32_t)GizmoTransformEditMode::Scale_Y;
constexpr uint32_t ScaleFlag_Z = (uint32_t)GizmoTransformEditMode::Scale_Z;

namespace Editor {
	ScenePanel::ScenePanel()
	: m_sceneCamera(0.0, 85.0f, .001f, 1000.0f, MSAASamples::Four), Panel("Scene") {
		auto& layer = App::get().GetLayer();

		TextureSettings gameObejctBufferSpecs = TextureFormat::R32UI;
		gameObejctBufferSpecs.filtering = TextureFiltering::Nearest;

		m_sceneCamera.getCamera().getRenderFrameBuffer()->attachColorTexture(gameObejctBufferSpecs);
		if (m_sceneCamera.getCamera().HasResultFrameBuffer()) {
			m_sceneCamera.getCamera().getResultFrameBuffer()->attachColorTexture(gameObejctBufferSpecs);
		}
	

		// toogle translate (alt + g)
		layer.AddShortCut(Shortcut("Gizmos - Toogle Translate", ([&]() {
			m_gizmoMode ^= TranslateFlag;
			return true;
		}), Keyboard::G, false, false, true));

		// toogle rotate (alt + r)
		layer.AddShortCut(Shortcut("Gizmos - Toogle Rotate", ([&]() {
			m_gizmoMode ^= RotateFlag;
			return true;
		}), Keyboard::R, false, false, true));

		// toogle scale  (alt + s)
		layer.AddShortCut(Shortcut("Gizmos - Toogle Scale", ([&]() {
			m_gizmoMode ^= ScaleFlag;
			return true;
		}), Keyboard::S, false, false, true));

		// set only translate (g)
		layer.AddShortCut(Shortcut("Gizmos - Translate", ([&]() {
			m_gizmoMode = TranslateFlag;
			return true;
		}), Keyboard::G, false, false, false));

		// set only rotate (r)
		layer.AddShortCut(Shortcut("Gizmos - Rotate", ([&]() {
			m_gizmoMode = RotateFlag;
			return true;
		}), Keyboard::R, false, false, false));

		// set only scale (s)
		layer.AddShortCut(Shortcut("Gizmos - Scale", ([&]() {
			m_gizmoMode = ScaleFlag;
			return true;
		}), Keyboard::S, false, false, false));

		// disable all (q)
		layer.AddShortCut(Shortcut("Gizmos - Disable Gizmos", ([&]() {
			m_gizmoMode = NoneFlag;
			return true;
		}), Keyboard::Q, false, false, false));

		// enable all (u)
		layer.AddShortCut(Shortcut("Gizmos - Use Universal Gizmo", ([&]() {
			m_gizmoMode = Universal;
			return true;
		}), Keyboard::U, false, false, false));


		// toogle x axis (alt + x)
		layer.AddShortCut(Shortcut("Gizmos - Toggle X Axis", ([&]() {
			if (m_gizmoMode & TranslateFlag) {
				m_gizmoMode ^= TranslateFlag_X;
			}
			if (m_gizmoMode & RotateFlag) {
				m_gizmoMode ^= RotateFlag_X;
			}
			if (m_gizmoMode & ScaleFlag) {
				m_gizmoMode ^= ScaleFlag_X;
			}
			return true;
		}), Keyboard::X, false, false, true));

		// toogle y axis (alt + y)
		layer.AddShortCut(Shortcut("Gizmos - Toggle Y Axis", ([&]() {
			if (m_gizmoMode & TranslateFlag) {
				m_gizmoMode ^= TranslateFlag_Y;
			}
			if (m_gizmoMode & RotateFlag) {
				m_gizmoMode ^= RotateFlag_Y;
			}
			if (m_gizmoMode & ScaleFlag) {
				m_gizmoMode ^= ScaleFlag_Y;
			}
			return true;
		}), Keyboard::Y, false, false, true));

		// toogle z axis (alt + z)
		layer.AddShortCut(Shortcut("Gizmos - Toggle Z Axis", ([&]() {
			if (m_gizmoMode & TranslateFlag) {
				m_gizmoMode ^= TranslateFlag_Z;
			}
			if (m_gizmoMode & RotateFlag) {
				m_gizmoMode ^= RotateFlag_Z;
			}
			if (m_gizmoMode & ScaleFlag) {
				m_gizmoMode ^= ScaleFlag_Z;
			}
			return true;
		}), Keyboard::Z, false, false, true));

		// only x axis (x)
		layer.AddShortCut(Shortcut("Gizmos - Use X Axis only", ([&]() {
			if (m_gizmoMode & TranslateFlag) {
				m_gizmoMode = m_gizmoMode & ~TranslateFlag;
				m_gizmoMode |= TranslateFlag_X;
			}
			if (m_gizmoMode & RotateFlag) {
				m_gizmoMode = m_gizmoMode & ~RotateFlag;
				m_gizmoMode |= RotateFlag_X;
			}
			if (m_gizmoMode & ScaleFlag) {
				m_gizmoMode = m_gizmoMode & ~ScaleFlag;
				m_gizmoMode |= ScaleFlag_X;
			}
			return true;
			}), Keyboard::X, false, false, false));

		// only y axis (y)
		layer.AddShortCut(Shortcut("Gizmos - Use Y Axis only", ([&]() {
			if (m_gizmoMode & TranslateFlag) {
				m_gizmoMode = m_gizmoMode & ~TranslateFlag;
				m_gizmoMode |= TranslateFlag_Y;
			}
			if (m_gizmoMode & RotateFlag) {
				m_gizmoMode = m_gizmoMode & ~RotateFlag;
				m_gizmoMode |= RotateFlag_Y;
			}
			if (m_gizmoMode & ScaleFlag) {
				m_gizmoMode = m_gizmoMode & ~ScaleFlag;
				m_gizmoMode |= ScaleFlag_Y;
			}
			return true;
			}), Keyboard::Y, false, false, false));

		// only z axis (z)
		layer.AddShortCut(Shortcut("Gizmos - Use Z Axis only", ([&]() {
			if (m_gizmoMode & TranslateFlag) {
				m_gizmoMode = m_gizmoMode & ~TranslateFlag;
				m_gizmoMode |= TranslateFlag_Z;
			}
			if (m_gizmoMode & RotateFlag) {
				m_gizmoMode = m_gizmoMode & ~RotateFlag;
				m_gizmoMode |= RotateFlag_Z;
			}
			if (m_gizmoMode & ScaleFlag) {
				m_gizmoMode = m_gizmoMode & ~ScaleFlag;
				m_gizmoMode |= ScaleFlag_Z;
			}
			return true;
			}), Keyboard::Z, false, false, false));
	}
	void ScenePanel::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Scene Panel");
		auto& layer = App::get().GetLayer();

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_width = (uint32_t)glm::max(viewportSize.x, 1.0f);
		m_height = (uint32_t)glm::max(viewportSize.y, 1.0f);

		auto& frameBuffer = m_sceneCamera.getCamera().getResultFrameBuffer();
		Gizmo::ApplyToFrameBuffer(frameBuffer);

		m_windowPos = ImGui::GetCursorScreenPos();

		ImGui::Image(frameBuffer->getColorAttachment(0).get(), viewportSize, ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));

		if (ImGui::IsItemHovered()) {
			const uint32_t clickX = (uint32_t)(ImGui::GetMousePos().x - m_windowPos.x);
			const uint32_t clickY = m_height - (uint32_t)(ImGui::GetMousePos().y - m_windowPos.y);
			entt::entity id = (entt::entity)frameBuffer->getColorAttachment(1)->getPixel(clickX, clickY);
			if (layer.GetActiveScene()->IsValid(id)) {
				m_hoveredObject = id;
			}
		}


		FrameBufferSpecs FBspec = frameBuffer->getSpecs();
		if (FBspec.width != m_width || FBspec.height != m_height)
			m_sceneCamera.onResize((float)m_width, (float)m_height);

		glm::vec2 windowPos = glm::vec2(ImGui::GetCurrentWindow()->WorkRect.Min.x, ImGui::GetCurrentWindow()->WorkRect.Min.y);
		Gizmo::setRect(windowPos.x, windowPos.y, (float)m_width, (float)m_height);
		Gizmo::setCamData(m_sceneCamera.getCamera().getProjectionMatrix(), glm::inverse(m_sceneCamera.getTransform().GetWorldTransform()));
		Gizmo::setWindow(GetID(), &OpenPtr());

		Stulu::UUID sceneUUID = Controls::ReceiveDragDopAsset(AssetsManager::GlobalInstance().GetTypeNameT<SceneAsset>());
		if (sceneUUID != Stulu::UUID::null) {
			SceneAsset asset = AssetsManager::GlobalInstance().GetAsset<SceneAsset>(sceneUUID);
			layer.OpenScene(asset.Path());
		}

		Stulu::UUID materialUUID = Controls::ReceiveDragDopAsset(AssetsManager::GlobalInstance().GetTypeNameT<MaterialAsset>());
		if (materialUUID != Stulu::UUID::null && m_hoveredObject != entt::null) {
			MaterialAsset asset = AssetsManager::GlobalInstance().GetAsset<MaterialAsset>(materialUUID);
			GameObject go = { m_hoveredObject, layer.GetActiveScene().get()};
			if (go.hasComponent<MeshRendererComponent>()) {
				go.getComponent<MeshRendererComponent>().material = asset;
			}
		}
	}
	void ScenePanel::DrawImGuizmo() {
		auto& layer = App::get().GetLayer();
		auto& scene = layer.GetActiveScene();
		auto& sceneCamera = GetCamera();

		if (m_drawGrid) {
			Gizmo::drawGrid(glm::mat4(1.0f), 100.0f);
		}

		const std::vector<entt::entity> selectedObjs = layer.GetPanel<HierarchyPanel>().GetSelected();

		if(selectedObjs.size() > 0 && m_gizmoMode != NoneFlag) {
			const float baseSize = glm::min(128.0f, m_height / 4.0f);
			const ImVec2 size = ImVec2(baseSize, baseSize);
			const ImVec2 pos = m_windowPos + ImVec2(m_width - m_windowPadding.x - baseSize, m_windowPadding.y);

			// averages
			glm::vec3 avgPos(0.0f);
			{
				for (const entt::entity& entID : selectedObjs) {
					auto& tc = GameObject(entID, scene.get()).getComponent<TransformComponent>();
					avgPos += tc.GetWorldPosition();
				}
				avgPos /= (float)selectedObjs.size();
			}

			if (Gizmo::ViewManipulate(sceneCamera.getTransform(), avgPos, pos, size)) {}
		}
		

		
		const glm::vec3& cameraPos = sceneCamera.getTransform().GetWorldPosition();
		const float gizmoViewDistance = 50.0f;
		//draw all cameras
		for (entt::entity goID : scene->GetAllWith<CameraComponent>()) {
			GameObject go = GameObject(goID, scene.get());
			TransformComponent& transf = go.getComponent<TransformComponent>();
			float camDistance = glm::distance(cameraPos, transf.GetWorldPosition());

			if (camDistance <= gizmoViewDistance)
				Renderer2D::drawTexturedQuad(Math::createMat4(transf.GetWorldPosition(), transf.GetWorldRotation(),
					glm::vec3(.75f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))), Resources::GetCameraTexture());
		}
		//draw all lights
		for (entt::entity goID : scene->GetAllWith<LightComponent>()) {
			GameObject go = GameObject(goID, scene.get());
			TransformComponent& transf = go.getComponent<TransformComponent>();
			LightComponent& light = go.getComponent<LightComponent>();
			float camDistance = glm::distance(cameraPos, transf.GetWorldPosition());

			if (camDistance <= gizmoViewDistance) {
				switch (light.lightType)
				{
				case LightComponent::Area:
					Gizmo::drawTextureBillBoard(Resources::GetLightTexture(), transf.GetWorldPosition(), glm::vec3(.75f), glm::vec3(0, 1, 0), glm::vec4(light.color, 1.0f));
					break;
				case LightComponent::Directional:
					Renderer2D::drawTexturedQuad(
						Math::createMat4(transf.GetWorldPosition(), transf.GetWorldRotation(), glm::vec3(1.5f, .75f, 1.5f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))),
						Resources::GetDirectionalLightTexture(), glm::vec2(1.0f), glm::vec4(light.color, 1.0f));
					break;
				case LightComponent::Spot:
					Renderer2D::drawTexturedQuad(
						Math::createMat4(transf.GetWorldPosition(), transf.GetWorldRotation(), glm::vec3(.75f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))),
						Resources::GetSpotLightTexture(), glm::vec2(1.0f), glm::vec4(light.color, 1.0f));
					break;
				}
			}
		}
		if (layer.IsRuntime() && scene->getCaller()->HasLayer<PhysicsScene>()) {
			scene->getCaller()->GetLayer<PhysicsScene>().RenderSceneDebugData();
		}
	}

	void ScenePanel::Update() {
		auto& layer = App::get().GetLayer();

		if (!Gizmo::WasUsed()) {
			Input::setEnabled(IsFocused());
			m_sceneCamera.updateMove(Time::frameTime);
			m_sceneCamera.onUpdate(Time::frameTime);

			Input::setEnabled(true);
		}

		if (IsFocused() && IsVisible()) {
			Input::setCursorMode(Input::CursorMode::Normal);
		}


		if (IsVisible()) {
			auto& cam = m_sceneCamera.getCamera();

			// set draw buffers, for entity texture buffer, if no result buffer present will fallback to renderbuffer
			cam.getRenderFrameBuffer()->SetDrawBuffers({ 0,1 });
			layer.GetActiveScene()->onUpdateEditor(GetCamera(), true);
			cam.getRenderFrameBuffer()->SetDrawBuffer();

			layer.GetActiveScene()->getRenderer()->ApplyPostProcessing(GetCamera());
			
			if (cam.HasResultFrameBuffer()) {
				// blit gameobject id map 
				cam.getRenderFrameBuffer()->SetReadBuffer(1);
				cam.getResultFrameBuffer()->SetDrawBuffer(1);
				
				Renderer::BlibRenderBuffferToResultBuffer(cam.getRenderFrameBuffer(), cam.getResultFrameBuffer(), true, false, false);

				cam.getRenderFrameBuffer()->SetReadBuffer();
				cam.getResultFrameBuffer()->SetDrawBuffer();
			}
		}
		else {
			layer.GetActiveScene()->onUpdateEditor(GetCamera(), false);
		}

		m_hoveredObject = entt::null;
	}


	void ScenePanel::OnEvent(Stulu::Event& e) {
		if (IsFocused()) {
			EventDispatcher dispatcher(e);
			dispatcher.dispatch<MouseButtonDownEvent>(ST_BIND_EVENT_FN(ScenePanel::OnMouseDown));

			m_sceneCamera.onEvent(e);
		}
	}
	bool ScenePanel::OnMouseDown(Stulu::MouseButtonDownEvent& e) const {
		if (e.getButton() == Mouse::ButtonLeft) {
			if (m_hoveredObject != entt::null) {
				App::get().GetLayer().GetPanel<HierarchyPanel>().SelectedLogicAdd(m_hoveredObject);
				return true;
			}
		}
		return false;
	}

	void ScenePanel::PreWindow() {
		m_windowPadding = ImGui::GetStyle().WindowPadding;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	}
	void ScenePanel::PostWindow() {
		ImGui::PopStyleVar();

		if (!IsVisible()) {
			return;
		}
		
		DrawMenuBars(m_windowPos, true);
	}

	
	void ScenePanel::DrawMenuBars(ImVec2 startPos, bool showToolbar, float parentWindowWidth) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVarY(ImGuiStyleVar_WindowPadding, 3.0f);
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 4.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushFontLarge();

		const ImVec4 selectedColor = ImGui::GetStyle().Colors[ImGuiCol_TextLink];
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;

		if (showToolbar) {
			ImGui::SetNextWindowBgAlpha(0.5f);
			ImGui::SetNextWindowPos(startPos + m_windowPadding);

			if (ImGui::Begin("ToolbarEdit", nullptr, flags)) {
				bool selected = false;

				selected = m_gizmoMode == NoneFlag;
				if (selected) {
					ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
				}
				if (ImGui::Button(ICON_FK_MOUSE_POINTER)) {
					m_gizmoMode = NoneFlag;
				}
				if (selected) {
					ImGui::PopStyleColor();
				}
				ImGui::SameLine();

				selected = m_gizmoMode & TranslateFlag;
				if (selected) {
					ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
				}
				if (ImGui::Button(ICON_FK_ARROWS)) {
					m_gizmoMode ^= TranslateFlag;
				}
				if (selected) {
					ImGui::PopStyleColor();
				}
				ImGui::SameLine();


				selected = m_gizmoMode & RotateFlag;
				if (selected) {
					ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
				}
				if (ImGui::Button(ICON_FK_REFRESH)) {
					m_gizmoMode ^= RotateFlag;
				}
				if (selected) {
					ImGui::PopStyleColor();
				}
				ImGui::SameLine();


				selected = m_gizmoMode & ScaleFlag;
				if (selected) {
					ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
				}
				if (ImGui::Button(ICON_FK_ARROWS_ALT)) {
					m_gizmoMode ^= ScaleFlag;
				}
				if (selected) {
					ImGui::PopStyleColor();
				}

				//selected = m_drawGrid;
				//if (selected) {
				//	ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
				//}
				//if (ImGui::Button(ICON_FK_TABLE)) {
				//	m_drawGrid = !m_drawGrid;
				//}
				//if (selected) {
				//	ImGui::PopStyleColor();
				//}
			}
			ImGui::End();
		}


		auto& layer = App::get().GetLayer();
		bool isRuntime = layer.IsRuntime();
		bool isPaused = Time::Scale == 0.0f;

		if (parentWindowWidth < 0.0f) parentWindowWidth = (float)m_width;

		ImGui::SetNextWindowBgAlpha(0.5f);
		ImVec2 pos = ImVec2(startPos.x + (parentWindowWidth * 0.5f), startPos.y + m_windowPadding.y);
		ImGui::SetNextWindowPos(pos, 0, ImVec2(0.5f, 0.0f));

		if (ImGui::Begin("ToolBarRuntime", nullptr, flags)) {
			if (isRuntime) {
				ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
				if (ImGui::Button(ICON_FK_STOP)) {
					layer.StopRuntime();
				}
				ImGui::PopStyleColor();
			}
			else {
				if (ImGui::Button(ICON_FK_PLAY)) {
					layer.StartRuntime();
				}
			}
			ImGui::SameLine();

			if (isPaused) {
				ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
				if (ImGui::Button(ICON_FK_FORWARD)) {
					Time::Scale = 1.0f;
				}
				ImGui::PopStyleColor();
			}
			else {
				if (ImGui::Button(ICON_FK_PAUSE)) {
					Time::Scale = 0.0f;
				}
			}
		}
		ImGui::End();


		ImGui::PopFont();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(2);
	}
	
	
}
