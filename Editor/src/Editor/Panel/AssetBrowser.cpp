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
				if (ImGui::Button("<-")) {
					if (m_path.parent_path() == std::filesystem::path("Stulu/assets")) {
						m_path = getEditorProject().assetPath;
					}
					else {
						m_path = m_path.parent_path();
					}
				}
				ImGui::SameLine();
			}
			ImGui::Text("%s", m_path.string().c_str());
			static float icoSize = 70.0f;
			float itemSize = icoSize + 22.0f;
			float width = ImGui::GetContentRegionAvailWidth();
			int cCount = std::max((int)(width / itemSize), 1);
			ImGui::Separator();
			ImGui::Columns(cCount, 0, false);
			for (auto& directory : std::filesystem::directory_iterator(m_path)) {
				std::string extension = directory.path().extension().string();
				if (extension == ".meta")
					continue;
				const auto& path = directory.path();
				const auto& filename = path.filename();
				auto relativePath = std::filesystem::relative(path, getEditorProject().assetPath);

				{
					const Ref<Texture>& ico = getIcon(directory);
					ImVec4 icoColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
					ImVec4 bgColor = { 0,0,0,0 };
					if(selected.path == path.string())
						bgColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive);
					if (extension == ".mat") {
						icoColor = ImVec4(1, 1, 1, 1);
					}
					ImGui::PushID(filename.string().c_str());
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::ImageButton(reinterpret_cast<void*>((uint64_t)ico->getRendererID()), { icoSize, icoSize }, { 0, 1 }, { 1, 0 }, -1, bgColor, icoColor);
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
				}
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
					if (!directory.is_directory())
						selected = AssetsManager::get(AssetsManager::getFromPath(path.string()));
				}
				if (ImGui::BeginPopupContextWindow("ASSET_BROWSER_RIGHT_CLICK_PROPS")) {
					if (ImGui::BeginMenu("Create")) {
						if (ImGui::MenuItem("Material")) {
							
						}
						ImGui::EndMenu();
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Delete")) {
						AssetsManager::remove(selected.uuid);
						std::remove(selected.path.c_str());
					}
					ImGui::EndPopup();
				}
				ImGui::TextWrapped(filename.string().c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Columns(1);
		}
		ImGui::End();
		if (ImGui::Begin("Directorys"), open) {
			if (ImGui::CollapsingHeader("Project Assets")) {
				drawDirectory(getEditorProject().assetPath);
			}
			if(ImGui::CollapsingHeader("Engine Assets")) {
				drawDirectory("Stulu/assets/SkyBox", true);
				drawDirectory("Stulu/assets/Shaders", true);
				drawDirectory("Stulu/assets/Textures", true);
			}
		}
		ImGui::End();
		if(m_inspector)
			renderInspector();
	}
	void AssetBrowserPanel::renderInspector() {
		if (ImGui::Begin("Asset Inspector", &m_inspector)) {
			ImGui::TextWrapped("Asset %s", selected.path.c_str());
			ImGui::Text("UUID %d", selected.uuid);
			ImGui::Text("Properitys:", selected.path.c_str());
			if (ImGui::BeginChild("Properitys") && selected.uuid != UUID::null) {
				if (selected.type == AssetType::Model) {
					std::vector<MeshAsset>& vec = std::any_cast<Model>(selected.data).getMeshes();
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
				}
				else if (selected.type == AssetType::Material) {
					ComponentsRender::drawMaterialEdit("Material", selected.uuid, false);
				}
				else if (selected.type == AssetType::Texture) {
					int type = (int)selected.type - 1;
					if (ComponentsRender::drawComboControl("Texture type", type, "Texture2D\0Texture")) {
						selected.type = AssetType::Texture2D;
						AssetsManager::update(selected.uuid, selected);
					}
				}
				else if (selected.type == AssetType::Texture2D) {
					int type = (int)selected.type - 1;
					if (ComponentsRender::drawComboControl("Texture type", type, "Texture2D\0Texture")) {
						selected.type = AssetType::Texture;
						AssetsManager::update(selected.uuid, selected);
					}
					else {
						ComponentsRender::drawVector2Control("Tilling", std::any_cast<Ref<Texture2D>>(selected.data)->getSettings().tiling);
						ComponentsRender::drawComboControl("Format", std::any_cast<Ref<Texture2D>>(selected.data)->getSettings().format, "RGBA\0RGB\0RG\0A\0SRGB\0SRGBA\0RGBA16F\0RGB16F\0Auto");
						ComponentsRender::drawComboControl("Wrap Mode", std::any_cast<Ref<Texture2D>>(selected.data)->getSettings().wrap, "Clamp\0Repeat");
						if (ImGui::Button("Update")) {
							std::any_cast<Ref<Texture2D>>(selected.data)->update();
							AssetsManager::setProperity<TextureSettings>(selected.path, std::pair<std::string, TextureSettings> {"format", std::any_cast<Ref<Texture2D>>(selected.data)->getSettings()});
						}
					}
				}
			}
			ImGui::EndChild();
			ImGui::Separator();
			if (ImGui::Button("Recompile Shaders")) {
				AssetsManager::reloadShaders(getEditorProject().assetPath);
			}
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
			return (Ref<Texture>&)std::any_cast<Ref<Texture2D>&>(asset.data);
			break;
		case Stulu::AssetType::CubeMap:
			return Previewing::get().get(asset.uuid);
			break;
		case Stulu::AssetType::Model:
			return EditorResources::getObjectTexture();
			break;
		case Stulu::AssetType::Mesh:
			return EditorResources::getFileTexture();
			break;
		case Stulu::AssetType::Material:
			return Previewing::get().get(asset.uuid);
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
	void AssetBrowserPanel::deletePath(const std::filesystem::directory_entry& _directory) {
		for (auto& directory : std::filesystem::directory_iterator(_directory)) {
			if (directory.is_directory()) {
				deletePath(directory);
				continue;
			}
			AssetsManager::remove(AssetsManager::getFromPath(directory.path().string()));
		}
		std::remove(_directory.path().string().c_str());
	}
	void AssetBrowserPanel::drawDirectory(const std::filesystem::path& _path, bool includePathDir) {
		ST_PROFILING_FUNCTION();
		if (includePathDir) {
			ImGuiTreeNodeFlags flags = !containsDirSubDirs(_path) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNodeEx(_path.filename().string().c_str(), flags)) {
				drawDirectory(_path);
				ImGui::TreePop();
			}
			if (ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				m_path = _path;
			}
			return;
		}
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