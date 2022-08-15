#include "SceneViewport.h"
#include "Editor/Resources.h"
#include "Editor/EditorApp.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>


namespace Stulu {
	void SceneViewportPanel::draw(SceneCamera& cam, bool* open) {
		ST_PROFILING_FUNCTION();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
		if (ImGui::Begin("Scene", open)) {

			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			width = (uint32_t)viewportSize.x;
			height = (uint32_t)viewportSize.y;
			ImTextureID viewPortTexture;

			if(getEditorScene()->getData().shaderFlags & ST_ShaderViewFlags_DisplayDepth)
				viewPortTexture = reinterpret_cast<void*>((uint64_t)cam.getCamera()->getFrameBuffer()->getTexture()->getDepthAttachmentRendereID());
			else
				viewPortTexture = reinterpret_cast<void*>((uint64_t)cam.getDisplayBuffer()->getTexture()->getColorAttachmentRendereID());

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
				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Mesh)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					MeshAsset mesh = std::any_cast<MeshAsset>(AssetsManager::get(id).data);
					getEditorScene()->addMeshAssetToScene(mesh);
				}
				else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Model)).c_str())) {
					UUID id = *(UUID*)payload->Data;
					Model& model = std::any_cast<Model>(AssetsManager::get(id).data);
					GameObject go = getEditorScene()->addModel(model);
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


			glm::vec2 windowPos = glm::vec2(ImGui::GetCurrentWindow()->WorkRect.Min.x, ImGui::GetCurrentWindow()->WorkRect.Min.y);
			Gizmo::setRect(windowPos.x, windowPos.y, (float)width, (float)height);
			Gizmo::setCamData(cam.getCamera()->getProjectionMatrix(), glm::inverse(cam.getTransform().transform));
		}
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}
}