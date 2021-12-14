#include "AssetBrowser.h"
#include "imgui/imgui.h"
#include "Editor/Resources.h"
#include "Editor/EditorLayer.h"
#include "Editor/EditorApp.h"
#include "Editor/Panel/StyleEditor.h"

namespace Stulu {
	AssetBrowserPanel::AssetBrowserPanel(const std::filesystem::path& path)
		: m_path(path) {
		ST_PROFILING_FUNCTION();

	}
	void AssetBrowserPanel::render(bool* open) {
		ST_PROFILING_FUNCTION();
		if (ImGui::Begin("Assets"), open) {
			if (m_path != getEditorProject().assetPath) {
				if (ImGui::Button("<-"))
					m_path = m_path.parent_path();
			}

			static float icoSize = 70.0f;
			float itemSize = icoSize + 22.0f;
			float width = ImGui::GetContentRegionAvailWidth();
			int cCount = std::max((int)(width / itemSize), 1);

			ImGui::Columns(cCount, 0, false);
			for (auto& directory : std::filesystem::directory_iterator(m_path)) {
				std::string extension = directory.path().extension().string();
				if (extension == ".meta")
					continue;
				const auto& path = directory.path();
				const auto& filename = path.filename();
				auto relativePath = std::filesystem::relative(path, getEditorProject().assetPath);

				{
					Ref<Texture> ico = getIcon(directory);
					ImVec4 icoColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
					if (extension == ".mat") {
						icoColor = ImVec4(1, 1, 1, 1);
					}
					ImGui::PushID(filename.string().c_str());
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)ico->getRendererID()), { icoSize, icoSize }, { 0, 1 }, { 1, 0 }, -1, { 0,0,0,0 }, icoColor);
				}

				if (ImGui::BeginDragDropSource()) {
					UUID uuid = AssetsManager::getFromPath(path.string());
					ImGui::SetDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetsManager::getAssetType(uuid))).c_str(), &uuid, sizeof(UUID));
					ImGui::EndDragDropSource();
				}
				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					if (directory.is_directory())
						m_path /= filename;
					else {
						selected = AssetsManager::get(AssetsManager::getFromPath(path.string()));
					}
				}
				ImGui::TextWrapped(filename.string().c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Columns(1);
		}
		ImGui::End();
		if (ImGui::Begin("Directorys"), open) {
			drawDirectory(getEditorProject().assetPath);
		}
		ImGui::End();
		if(m_inspector)
			renderInspector();
	}
	void AssetBrowserPanel::renderInspector() {
		std::vector<MeshAsset> vec;
		int type = (int)selected.type - 1;
		if (selected.type == AssetType::Model)
			vec = std::any_cast<Model>(selected.data).getMeshes();
		if (ImGui::Begin("Asset Inspector", &m_inspector)) {
			ImGui::Text("Asset %s", selected.path.c_str());
			ImGui::Text("Properitys:", selected.path.c_str());
			if (ImGui::BeginChild("Properitys")) {
				switch (selected.type)
				{
				case Stulu::AssetType::Model:
					for (int i = 0; i < vec.size(); i++) {
						if (vec[i].name.c_str()) {
							ImGui::Text("Mesh: %s", vec[i].name.c_str());
							ImGui::PushID(vec[i].name.c_str());
						}
						else {
							ImGui::Text("Mesh: %d", i);
							ImGui::PushID(i);
						}
						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
							ImGui::SetDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::Mesh)).c_str(), &vec[i].uuid, sizeof(UUID));
							ImGui::EndDragDropSource();
						}
						ImGui::PopID();
					}
					break;
				case Stulu::AssetType::Material:
					ComponentsRender::drawMaterialEdit("Material", selected.uuid, false);
					break;
				case Stulu::AssetType::Texture:
					if (ComponentsRender::drawComboControl("Texture type", type, "Texture2D\0Texture")) {
						selected.type = AssetType::Texture2D;
						AssetsManager::update(selected.uuid, selected);
						break;
					}
					break;
				case Stulu::AssetType::Texture2D:
					if (ComponentsRender::drawComboControl("Texture type", type, "Texture2D\0Texture")) {
						selected.type = AssetType::Texture;
						AssetsManager::update(selected.uuid, selected);
						break;
					}
					ComponentsRender::drawVector2Control("Tilling", std::any_cast<Ref<Texture2D>>(selected.data)->tiling);
					break;
				default:
					break;
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}
	const Ref<Texture>& AssetBrowserPanel::getIcon(const std::filesystem::directory_entry& directory) {
		ST_PROFILING_FUNCTION();
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
			return (Ref<Texture>&)std::any_cast<Ref<Texture2D>&>(asset.data);
			break;
		case Stulu::AssetType::Texture:
			return (Ref<Texture>)std::any_cast<Ref<Texture2D>&>(asset.data);
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
	void AssetBrowserPanel::drawDirectory(const std::filesystem::path& _path) {
		ST_PROFILING_FUNCTION();
		for (auto& directory : std::filesystem::directory_iterator(_path)) {
			std::filesystem::path path = directory.path();
			if (!directory.is_directory())
				return;
			ImGuiTreeNodeFlags flags = !containsDirSubDirs(path) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNodeEx(path.filename().string().c_str(), flags)) {
				drawDirectory(path);
				ImGui::TreePop();
			}
			if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				m_path = path;
			}
		}
	}
	bool AssetBrowserPanel::containsDirSubDirs(std::filesystem::path path) {
		ST_PROFILING_FUNCTION();
		for (auto& directory : std::filesystem::directory_iterator(path)) {
			if (directory.is_directory())
				return true;
		}
		return false;
	}
}