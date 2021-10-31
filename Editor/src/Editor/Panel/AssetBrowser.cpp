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
				if (path.extension() == ".meta")
					continue;
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
					UUID uuid = AssetsManager::getFromPath(itemPathString);
					ImGui::SetDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetsManager::getAssetType(uuid))).c_str(), &uuid, sizeof(UUID));
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (directory.is_directory())
						m_path /= path.filename();
					else {
						if (extension == ".glb" || extension == ".obj") {

							GameObject go = Model::loadModel(path.string(), EditorLayer::getActiveScene().get());	
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
		if (ImGui::Begin("Directorys"), open) {
			drawDirectory("assets");
		}
		ImGui::End();
	}
	const Ref<Texture>& AssetBrowserPanel::getIcon(const std::filesystem::directory_entry& directory) {
		if (directory.is_directory()) {
			return EditorResources::getFolderTexture();
		}
		std::string path = directory.path().string();
		auto& asset = AssetsManager::get(AssetsManager::getFromPath(path));
		
		switch (asset.type)
		{
		case Stulu::AssetType::Unknown:
			return EditorResources::getFileTexture();
			break;
		case Stulu::AssetType::Texture2D:
			return *asset.data._Cast<Ref<Texture2D>>();
			break;
		case Stulu::AssetType::Texture:
			return *asset.data._Cast<Ref<Texture>>();
			break;
		case Stulu::AssetType::Model:
			return EditorResources::getObjectTexture();
			break;
		case Stulu::AssetType::Mesh:
			return EditorResources::getFileTexture();
			break;
		case Stulu::AssetType::Material:
			return Previewing::get().getMaterial(asset.uuid);
			break;
		case Stulu::AssetType::Shader:
			return EditorResources::getFileTexture();
			break;
		case Stulu::AssetType::Scene:
			return EditorResources::getSceneTexture();
			break;
		default:
			break;
		}
		return EditorResources::getFileTexture();
	}
	void AssetBrowserPanel::drawDirectory(const std::string& path) {
		for (auto& directory : std::filesystem::directory_iterator(path)) {
			std::filesystem::path path = directory.path();
			if (!directory.is_directory())
				return;
			ImGuiTreeNodeFlags flags = (std::filesystem::is_empty(path) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow) | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNodeEx(path.filename().string().c_str(), flags)) {
				drawDirectory(path.string());
				ImGui::TreePop();
			}
			if (ImGui::IsItemClicked()) {
				m_path = path;
			}
		}
	}
}