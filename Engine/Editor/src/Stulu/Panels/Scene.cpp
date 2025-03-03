#include "Scene.h"
#include <imgui_internal.h>
#include <Stulu/ImGui/Gizmo.h>

#include "Stulu/App.h"
#include "Stulu/Controls.h"
#include "Stulu/Style.h"


using namespace Stulu;

namespace Editor {
	ScenePanel::ScenePanel()
	: m_sceneCamera(0.0, 85.0f, .001f, 1000.0f, 1), Panel("Scene") {

	}
	void ScenePanel::DrawImGui() {
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		m_width = (uint32_t)glm::max(viewportSize.x, 1.0f);
		m_height = (uint32_t)glm::max(viewportSize.y, 1.0f);

		FrameBufferSpecs FBspec = m_sceneCamera.getCamera()->getFrameBuffer()->getSpecs();
		if (FBspec.width != m_width || FBspec.height != m_height)
			m_sceneCamera.onResize((float)m_width, (float)m_height);

		m_focused = ImGui::IsWindowFocused();

		Gizmo::ApplyToFrameBuffer(m_sceneCamera.getCamera()->getFrameBuffer());

		m_windowPos = ImGui::GetCursorScreenPos();

		auto& texture = m_sceneCamera.getCamera()->getFrameBuffer()->getColorAttachment();
		ImGui::Image(texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));

		glm::vec2 windowPos = glm::vec2(ImGui::GetCurrentWindow()->WorkRect.Min.x, ImGui::GetCurrentWindow()->WorkRect.Min.y);
		Gizmo::setRect(windowPos.x, windowPos.y, (float)m_width, (float)m_height);
		Gizmo::setCamData(m_sceneCamera.getCamera()->getProjectionMatrix(), glm::inverse(m_sceneCamera.getTransform().transform));

		Gizmo::setWindow(GetID(), &OpenPtr());

		Stulu::UUID sceneUUID = Controls::ReceiveDragDopAsset(AssetType::Scene);
		if (sceneUUID != Stulu::UUID::null) {
			Asset& asset = AssetsManager::get(sceneUUID);
			App::get().GetLayer().OpenScene(asset.path);
		}
	}
	void ScenePanel::DrawImGuizmo() {
		if (m_drawGrid) {
			const auto& tc = m_sceneCamera.getTransform();
			const glm::vec3 pos = { glm::floor(tc.worldPosition.x), 0.0f, glm::floor(tc.worldPosition.y) };

			Gizmo::drawGrid(glm::translate(glm::mat4(1.0f), pos), 50.0f);
		}

		auto& scene = App::get().GetLayer().GetActiveScene();
		auto& sceneCamera = GetCamera();
		const glm::vec3& cameraPos = sceneCamera.getTransform().worldPosition;
		const float gizmoViewDistance = 50.0f;
		//draw all cameras
		for (entt::entity goID : scene->getAllGameObjectsWith<CameraComponent>()) {
			GameObject go = GameObject(goID, scene.get());
			TransformComponent& transf = go.getComponent<TransformComponent>();
			float camDistance = glm::distance(cameraPos, transf.worldPosition);

			if (camDistance <= gizmoViewDistance)
				Renderer2D::drawTexturedQuad(Math::createMat4(transf.worldPosition, transf.worldRotation,
					glm::vec3(.75f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))), Resources::GetCameraTexture());
		}
		//draw all lights
		for (entt::entity goID : scene->getAllGameObjectsWith<LightComponent>()) {
			GameObject go = GameObject(goID, scene.get());
			TransformComponent& transf = go.getComponent<TransformComponent>();
			LightComponent& light = go.getComponent<LightComponent>();
			float camDistance = glm::distance(cameraPos, transf.worldPosition);

			if (camDistance <= gizmoViewDistance) {
				switch (light.lightType)
				{
				case LightComponent::Area:
					Gizmo::drawTextureBillBoard(Resources::GetLightTexture(), transf.worldPosition, glm::vec3(.75f), glm::vec3(0, 1, 0), glm::vec4(light.color, 1.0f));
					break;
				case LightComponent::Directional:
					Renderer2D::drawTexturedQuad(
						Math::createMat4(transf.worldPosition, transf.worldRotation, glm::vec3(1.5f, .75f, 1.5f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))),
						Resources::GetDirectionalLightTexture(), glm::vec2(1.0f), glm::vec4(light.color, 1.0f));
					break;
				case LightComponent::Spot:
					Renderer2D::drawTexturedQuad(
						Math::createMat4(transf.worldPosition, transf.worldRotation, glm::vec3(.75f)) * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, -90, 0)))),
						Resources::GetSpotLightTexture(), glm::vec2(1.0f), glm::vec4(light.color, 1.0f));
					break;
				}
			}
		}
	}

	void ScenePanel::Update() {
		if (!Gizmo::IsUsing()) {
			Input::setEnabled(IsFocused());
			m_sceneCamera.updateMove(Time::frameTime);
			m_sceneCamera.onUpdate(Time::frameTime);

			App::get().GetLayer().GetActiveScene()->updateTransform(m_sceneCamera.getTransform());
			Input::setEnabled(true);
		}
	}


	void ScenePanel::OnEvent(Stulu::Event& e) {
		if(m_focused)
			m_sceneCamera.onEvent(e);
	}
	void ScenePanel::PostWindow() {

		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVarY(ImGuiStyleVar_WindowPadding, 3.0f);
		ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 4.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
		ImGui::PushFontLarge();

		const ImVec4 selectedColor = ImGui::GetStyle().Colors[ImGuiCol_TextLink];
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove;

		const uint32_t NoneFlag = (uint32_t)GizmoTransformEditMode::None;
		const uint32_t TranslateFlag = (uint32_t)GizmoTransformEditMode::Translate;
		const uint32_t RotateFlag = (uint32_t)GizmoTransformEditMode::Rotate;
		const uint32_t ScaleFlag = (uint32_t)GizmoTransformEditMode::Scale;

		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowPos(m_windowPos + ImGui::GetStyle().WindowPadding);

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



		auto& layer = App::get().GetLayer();
		bool isRuntime = layer.IsRuntime();
		bool isPaused = Time::Scale == 0.0f;

		ImGui::SetNextWindowBgAlpha(0.5f);
		ImVec2 pos = ImVec2(m_windowPos.x + (m_width * 0.5f), m_windowPos.y + ImGui::GetStyle().WindowPadding.y);
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
