#include "AssetBrowser.h"
#include "imgui/imgui.h"
#include "Editor/Resources.h"
#include "Editor/EditorLayer.h"
#include "Editor/Panel/StyleEditor.h"

namespace Stulu {
	AssetBrowserPanel::AssetBrowserPanel(const std::filesystem::path& path)
		: m_path(path) {
		
	}
	void AssetBrowserPanel::render(bool* open) {
		if (ImGui::Begin("Assets"), open) {
			if (m_path != std::filesystem::path("assets")) {
				if (ImGui::Button("<-"))
					m_path = m_path.parent_path();
			}

			static float icoSize = 70.0f;
			float itemSize = icoSize + 22.0f;
			float width = ImGui::GetContentRegionAvailWidth();
			int cCount = std::max((int)(width / itemSize), 1);

			ImGui::Columns(cCount, 0, false);
			for (auto& directory : std::filesystem::directory_iterator(m_path)) {
				const auto& path = directory.path();
				auto relativePath = std::filesystem::relative(path, "assets");
				std::string filenameString = relativePath.filename().string();

				std::string extension = directory.path().extension().string();

				Ref<Texture> ico = getIcon(directory);
				ImVec4 icoColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);

				if (extension == ".mat") {
					icoColor = ImVec4(1, 1, 1, 1);
				}

				ImGui::PushID(filenameString.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

				ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)ico->getRendererID()), { icoSize, icoSize }, { 0, 1 }, { 1, 0 }, -1, {0,0,0,0}, icoColor);

				if (ImGui::BeginDragDropSource()) {

					std::string itemPathString = "assets\\" + relativePath.string();
					const char* itemPath = itemPathString.c_str();

					if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" || extension == ".tga") {
						ImGui::SetDragDropPayload("DRAG_DROP_TEXTURE2D", itemPath, (strlen(itemPath) + 1) * sizeof(char));
					}
					else if (extension == ".scene" || extension == ".stulu") {
						ImGui::SetDragDropPayload("DRAG_DROP_SCENE", itemPath, (strlen(itemPath) + 1) * sizeof(char));
					}
					else if (extension == ".mat" || extension == ".material") {
						ImGui::SetDragDropPayload("DRAG_DROP_MATERIAL", itemPath, (strlen(itemPath) + 1) * sizeof(char));
					}
					else {
						ImGui::SetDragDropPayload("ASSETS_BROWSER_DRAG_DROP", itemPath, (strlen(itemPath) + 1) * sizeof(char));
					}
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (directory.is_directory())
						m_path /= path.filename();
					else {
						if (extension == ".glb" || extension == ".obj") {

							GameObject go = Model::loadModel(path.string(), EditorLayer::getActiveScene().get(), nullptr);	
						}
					}
				}
				ImGui::TextWrapped(filenameString.c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Columns(1);
		}
		ImGui::End();
	}
	const Ref<Texture> AssetBrowserPanel::getIcon(const std::filesystem::directory_entry& directory) {
		if (directory.is_directory()) {
			return EditorResources::getFolderTexture();
		}
		std::string extension = directory.path().extension().string();
		std::string path = directory.path().string();
		if (extension == ".mat" || extension == ".material") {
			return Previewing::get().getMaterial(path);
		}
		if (extension == ".obj" || extension == ".glb") {
			return EditorResources::getObjectTexture();
		}
		if (extension == ".scene" || extension == ".stulu") {
			return EditorResources::getSceneTexture();
		}
		return EditorResources::getFileTexture();
	}
}