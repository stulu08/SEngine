#include "AssetBrowser.h"
#include "Editor/Resources.h"
#include "Editor/EditorLayer.h"
#include "Editor/EditorApp.h"
#include "Editor/Panel/StyleEditor.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <ImGui/misc/cpp/imgui_stdlib.h>
namespace Stulu {
	AssetBrowserPanel::AssetBrowserPanel(const std::filesystem::path& path)
		: m_path(path) { }
	static std::unordered_map<std::string, UUID> pathCache;

	static std::string createPopUpFileName = "";
	static std::string createPopUpFileContent = "";
	static std::function<void()> createPopUpUIFunc = [=] {
		ImGui::InputText("Name", &createPopUpFileName);
	};
	static std::function<void(const std::string&, const std::string&)> onCreatePopUpFile = [=](const std::string& currentDirectory, const std::string& fileName) {
		ST_INFO("Creating new File at: {0}", currentDirectory + "/" + fileName);
		FILE* file = fopen((currentDirectory + "/" + fileName).c_str(), "a");
		fprintf(file, createPopUpFileContent.c_str());
		fclose(file);
	};
	static float icoSize = 70.0f;

	void AssetBrowserPanel::render(bool* open) {
		ST_PROFILING_FUNCTION();
		std::string pathBefore = m_path.string();
		bool openCreatePopUp = false;
		if (ImGui::Begin("Assets"), open) {
			if (m_path != getEditorProject().assetPath) {
				if (ImGui::Button("<-")) {
					if (m_path.parent_path() == std::filesystem::path(Application::getEngineAssetDir())) {
						m_path = getEditorProject().assetPath;
					}
					else {
						m_path = m_path.parent_path();
					}
				}
				ImGui::SameLine();
			}
			ImGui::Text("%s", m_path.string().c_str());
			float itemSize = icoSize + 22.0f;
			float width = ImGui::GetContentRegionAvail().x;
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
					if (selected.path == path.string())
						bgColor = ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive);
					if (extension == ".mat") {
						icoColor = ImVec4(1, 1, 1, 1);
					}
					ImGui::PushID(filename.string().c_str());
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

					ImGui::ImageButton(ico, { icoSize, icoSize }, { 0, 1 }, { 1, 0 }, -1, bgColor, icoColor);
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
				if ((ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)) && ImGui::IsItemHovered(ImGuiMouseButton_Right)) {
					selected = AssetsManager::get(AssetsManager::getFromPath(path.string(), AssetsManager::getAssetTypeByPath(path.string())));
					EditorInspectorPanel::AssetMode = true;
				}
				ImGui::TextWrapped(filename.string().c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}
			ImGui::Columns(1);

			if (ImGui::BeginPopupContextWindow("ASSET_BROWSER_RIGHT_CLICK_PROPS")) {
				if (ImGui::BeginMenu("Create")) {
					if (ImGui::MenuItem("Material")) {
						openCreatePopUp = true;
						createPopUpFileName = "New Material.mat";
						createPopUpFileContent = Resources::getDefaultMaterial()->toDataString(true);
						createPopUpUIFunc = [=] {
							ImGui::InputText("Name", &createPopUpFileName);
							ImGui::TextWrapped("Curently you can only edit Materials with an Text Editor");
						};
					}
					if (ImGui::MenuItem("Scene")) {
						openCreatePopUp = true;
						createPopUpFileName = "New Scene.scene";
						createPopUpFileContent = EditorResources::getDefaultSceneSource();
					}
					if (ImGui::MenuItem("Render Texture")) {
						openCreatePopUp = true;
						createPopUpFileName = "New Render Texture.srt";
						createPopUpFileContent = "Nothing to see here";
					}
					if (ImGui::MenuItem("Shader")) {
						openCreatePopUp = true;
						createPopUpFileName = "New Shader.glsl";
						createPopUpFileContent = ReadFile(Renderer::getShaderSystem()->GetEntry(Resources::getDefaultMaterial()->getShader()->getName())->GetPath());
					}
					if (ImGui::MenuItem("Script")) {
						openCreatePopUp = true;
						createPopUpFileName = "NewScriptClass";
						createPopUpFileContent = "";
						onCreatePopUpFile = [=](const std::string& currentDirectory, const std::string& fileName) {
							ST_INFO("Creating new File at: {0}", currentDirectory + "/" + fileName + ".cs");
							FILE* file = fopen((currentDirectory + "/" + fileName + ".cs").c_str(), "a");
							createPopUpFileContent = "using Stulu;\nusing System.Collections;\npublic class " + fileName + " : Component {\n	public override void onStart() {\n\n	}\n	public override void onUpdate() {\n\n	}\n}";
							fprintf(file, createPopUpFileContent.c_str());
							fclose(file);
						};
					}
					if (ImGui::MenuItem("Directory")) {
						openCreatePopUp = true;
						createPopUpFileName = "New Directory";
						onCreatePopUpFile = [=](const std::string& currentDirectory, const std::string& fileName) {
							ST_INFO("Creating new Directory at: {0}", currentDirectory + "/" + fileName);
							Platform::createDirectory((currentDirectory + "/" + fileName).c_str());
						};
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete")) {
					AssetsManager::remove(selected.uuid, true, true);
					selected = NullAsset;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Recompile Shaders")) {
					Renderer::getShaderSystem()->ReloadShaders();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Reload Assembly")) {
					getEditorProject().rebuildAssembly();
				}
				ImGui::EndPopup();
			}

		}
		ImGui::End();

		if (openCreatePopUp)
			ImGui::OpenPopup("Create File");
		drawCreateFilePopUp();

		if (ImGui::Begin("Directorys"), open) {
			if (ImGui::CollapsingHeader("Project Assets")) {
				drawDirectory(getEditorProject().assetPath, true);
			}
			if (ImGui::CollapsingHeader("Engine Assets")) {
				Asset as = Resources::getDefaultSkyBoxAsset();
				ImGui::Button("Default Skybox");
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					UUID uuid = as.uuid;
					ImGui::SetDragDropPayload((std::string("DRAG_DROP_ASSET_") + std::to_string((int)AssetType::SkyBox)).c_str(), &uuid, sizeof(UUID));
					ImGui::EndDragDropSource();
				}
			}
		}
		ImGui::End();
		if (pathBefore != m_path.string())
			pathCache.clear();
	}

	void AssetBrowserPanel::drawCreateFilePopUp() {
		ST_PROFILING_FUNCTION();
		if (ImGui::BeginPopupModal("Create File", 0)) {
			createPopUpUIFunc();
			bool disabled = createPopUpFileName.empty();
			if (disabled) {
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}
			if (ImGui::Button("Create")) {
				onCreatePopUpFile(m_path.string(), createPopUpFileName);
				createPopUpFileName = "";
				createPopUpFileContent = "";
				createPopUpUIFunc = [=] { ImGui::InputText("Name", &createPopUpFileName); };
				onCreatePopUpFile = [=](const std::string& currentDirectory, const std::string& fileName) {
					ST_INFO("Creating new File at: {0}", currentDirectory + "/" + fileName);
					FILE* file = fopen((currentDirectory + "/" + fileName).c_str(), "a");
					fprintf(file, createPopUpFileContent.c_str());
					fclose(file);
				};
				ImGui::CloseCurrentPopup();
			}
			if (disabled)
			{
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) {
				createPopUpFileName = "";
				createPopUpFileContent = "";
				createPopUpUIFunc = [=] { ImGui::InputText("Name", &createPopUpFileName); };
				onCreatePopUpFile = [=](const std::string& currentDirectory, const std::string& fileName) {
					ST_INFO("Creating new File at: {0}", currentDirectory + "/" + fileName);
					FILE* file = fopen((currentDirectory + "/" + fileName).c_str(), "a");
					fprintf(file, createPopUpFileContent.c_str());
					fclose(file);
				};
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	const Ref<Texture>& AssetBrowserPanel::getIcon(const std::filesystem::directory_entry& directory) {
		std::string path = directory.path().string();
		if (pathCache.find(path) == pathCache.end() || !AssetsManager::exists(pathCache[path])) {
			UUID result = AssetsManager::getFromPath(path);
			if (result == UUID::null) {
				if (directory.is_directory()) {
					AssetsManager::addDirectory(path);
				}
				AssetsManager::loadAllFiles(m_path.string());
				return getIcon(directory);
			}
			pathCache[path] = result;
		}
		if (directory.is_directory()) {
			return EditorResources::getFolderTexture();
		}


		Asset& asset = AssetsManager::get(pathCache[path]);
		switch (asset.type)
		{
		case Stulu::AssetType::Unknown:
			return EditorResources::getFileTexture();
			break;
		case Stulu::AssetType::Texture2D:
			return std::any_cast<Ref<Texture>&>(asset.data);
			break;
		case Stulu::AssetType::RenderTexture:
			return std::any_cast<Ref<Texture>&>(asset.data);
			break;
		case Stulu::AssetType::SkyBox:
			return Previewing::get().get(asset.uuid);
			break;
		case Stulu::AssetType::Model:
			return Previewing::get().get(asset.uuid);
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
		case Stulu::AssetType::Script:
			return EditorResources::getScriptTexture();
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
				continue;
			ImGuiTreeNodeFlags flags = !containsDirSubDirs(path) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNodeEx(path.filename().string().c_str(), flags)) {
				drawDirectory(path);
				ImGui::TreePop();
			}
			if (ImGui::IsItemClicked()) {
				m_path = path;
			}
		}
	}
	bool AssetBrowserPanel::containsDirSubDirs(std::filesystem::path path) {
		for (auto& directory : std::filesystem::directory_iterator(path)) {
			if (directory.is_directory())
				return true;
		}
		return false;
	}
}