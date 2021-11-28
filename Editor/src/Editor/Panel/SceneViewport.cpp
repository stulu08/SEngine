#include "SceneViewport.h"
#include <imgui/imgui.h>

#include "Editor/Resources.h"
#include "Editor/EditorApp.h"


namespace Stulu {
	void SceneViewportPanel::draw(SceneCamera& cam, GameObject selected, bool* open) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		if (ImGui::Begin("Scene", open)) {

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			width = (uint32_t)viewportSize.x;
			height = (uint32_t)viewportSize.y;

			ImTextureID viewPortTexture;

			viewPortTexture = reinterpret_cast<void*>((uint64_t)cam.getCamera()->getFrameBuffer()->getTexture()->getRendererID());

			ImGui::Image(viewPortTexture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));


			if (ImGui::BeginDragDropTarget()) {
				bool news = false;
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Scene)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					std::string path = AssetsManager::get(id).path;
					ST_INFO("Received Scene: {0}", path);
					EditorApp::getLayer().OpenScene(path);
					news = true;
				}
				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Model)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					GameObject go = Model::loadModel(AssetsManager::get(id).path, EditorLayer::getActiveScene().get());
				}
				ImGui::EndDragDropTarget();
				if (news) {
					ImGui::End();
					ImGui::PopStyleVar();
					return;
				}
			}
			FrameBufferSpecs FBspec = cam.getCamera()->getFrameBuffer()->getSpecs();
			focused = ImGui::IsWindowFocused();
			hovered = ImGui::IsWindowHovered();
			if (width > 0 && height > 0 && (FBspec.width != width || FBspec.height != height))
				cam.onResize((float)width, (float)height);



			//imguizmo
			if (selected && gizmoEditType != -1 && !EditorApp::getLayer().isRuntime()) {
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				float windowWidth = (float)ImGui::GetWindowWidth();
				float windowHeight = (float)ImGui::GetWindowHeight();
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

				// Camera
				const glm::mat4& cameraProjection = cam.getCamera()->getProjectionMatrix();
				glm::mat4 cameraView = glm::inverse(cam.getTransform().getTransform());

				// Gameobject transform
				TransformComponent& tc = selected.getComponent<TransformComponent>();
				glm::mat4 transform = tc.getWorldSpaceTransform();

				ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)gizmoEditType, ImGuizmo::MODE::LOCAL, glm::value_ptr(transform));

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
		}
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}
}