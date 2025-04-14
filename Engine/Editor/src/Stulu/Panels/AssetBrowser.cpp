#include "AssetBrowser.h"

#include "Stulu/Style.h"
#include "Stulu/Controls.h"
#include <Stulu/Resources.h>
#include <Stulu/Resources/Resources.h>
#include <Stulu/Scene/AssetsManager.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <magic_enum/magic_enum.hpp>

using namespace Stulu;

namespace Editor {
	AssetBrowser::AssetBrowser(const std::filesystem::path& path)
		: m_path(path), m_root(path), Panel("Assets") {}

	void AssetBrowser::DrawImGui() {
		ST_PROFILING_SCOPE("ImGui - Asset Browser");

		const float minPanelSize = 100.0f;

		if (ImGui::BeginChild("DirectoryBrowser", ImVec2(minPanelSize, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders)) {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_indent.y);
			ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 1.0f);
			DrawDirectoryBrowser(m_root);
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();

		ImGui::SameLine(0.0f, 0.0f);

		if (ImGui::BeginChild("FileBrowser", ImVec2(0, 0), ImGuiChildFlags_Borders)) {
			ImGui::Indent(m_indent.x);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_indent.y);
			DrawBrowser();
		}
		ImGui::EndChild();

		DrawPopUps();
	}
	void AssetBrowser::PreWindow() {
		m_indent = ImGui::GetStyle().WindowPadding;
		m_rounding = ImGui::GetStyle().ChildRounding;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
	}

	void AssetBrowser::PostWindow() {
		ImGui::PopStyleVar(2);

		DrawCreateFileModal();
	}

	void AssetBrowser::DrawBrowser() {
		// top bar
		{
			const bool disabled = IsSelectedPath(m_root);
			if (disabled)
				ImGui::BeginDisabled();
			if (ImGui::Button(ICON_FK_BACKWARD)) {
				m_path = m_path.parent_path();
			}

			if (!disabled) {
				Stulu::UUID receivedUUID = Controls::ReceiveDragDopAsset(AssetType::Unknown, true);
				if (receivedUUID != Stulu::UUID::null) {
					MoveAsset(m_path.parent_path(), receivedUUID);
				}
			}

			if (disabled)
				ImGui::EndDisabled();

			ImGui::SameLine();
			const float targetWidth = ImGui::GetContentRegionAvail().x / 4.0f;
			ImGui::SetNextItemWidth(glm::min(glm::max(targetWidth, 200.0f), 300.0f));
			ImGui::InputTextWithHint("##SearchBar", ICON_FK_SEARCH " Search...", &m_search);

			ImGui::SameLine();

			std::string relPath = std::filesystem::relative(m_path, m_root).string();
			std::replace(relPath.begin(), relPath.end(), '\\', '/');
			
			ImGui::PushFontLarge();

			float yPos = ImGui::GetCursorPosY() - ImGui::GetStyle().FramePadding.y;

			size_t pos = 0;
			while (pos < relPath.size()) {
				size_t location = relPath.find('/', pos);

				if (location == std::string::npos)
					location = relPath.size();

				if (pos != 0) {
					ImGui::SameLine();
					ImGui::SetCursorPosY(yPos);
					ImGui::TextDisabled("/");

					ImGui::SameLine();
				}
				ImGui::SetCursorPosY(yPos);
				ImGui::TextDisabled("%s", relPath.substr(pos, location - pos).c_str());

				pos = location + 1;
			}
			ImGui::PopFont();
		}
		ImGui::Separator();

		float frameSize = m_iconSize + (ImGui::GetStyle().ItemSpacing.x * 2.0f);
		float width = ImGui::GetContentRegionAvail().x;
		int frameCount = std::max((int)(width / frameSize), 1);
		ImGui::BeginColumns("AssetBrowserCols", frameCount, ImGuiOldColumnFlags_NoBorder | ImGuiOldColumnFlags_NoResize);

		for (auto& directory : std::filesystem::directory_iterator(m_path)) {
			const auto& path = directory.path();
			if (path.extension() == ".meta")
				continue;

			FileInteractAction action = DrawFileFrame(path, IsSelected(path));

			if(action == FileInteractAction::Pressed) {
				SetSelcted(path);
			}
			if (action == FileInteractAction::Open && directory.is_directory()) {
				SetPath(path);
			}

			ImGui::NextColumn();
		}
		ImGui::EndColumns();
	}
	void AssetBrowser::DrawDirectoryBrowser(const std::filesystem::path& path) {
		for (auto& entry : std::filesystem::directory_iterator(path)) {
			if (!entry.is_directory())
				continue;
			const auto& entryPath = entry.path();
			const bool hasChilds = DirectoryContainsSub(entryPath, true);
			const std::string fileName = entry.path().filename().string();
			const std::string idString = entry.path().string();
			const ImGuiID treeID = ImGui::GetCurrentWindow()->GetID(idString.c_str());

			ImGuiTreeNodeFlags treeFlags =
				ImGuiTreeNodeFlags_SpanFullWidth |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_OpenOnArrow;
			if (!hasChilds)
				treeFlags |= ImGuiTreeNodeFlags_Leaf;


			std::stringstream display;

			if (IsSelectedPath(entryPath)) {
				treeFlags |= ImGuiTreeNodeFlags_Selected;
			}
			if (ImGui::TreeNodeGetOpen(treeID)) {
				display << ICON_FK_FOLDER_OPEN;
			}
			else {
				display << ICON_FK_FOLDER;
			}
			display << ' ' << fileName;
			
			bool open = ImGui::TreeNodeEx(idString.c_str(), treeFlags, display.str().c_str());

			if (ImGui::IsItemClicked()) {
				SetPath(entryPath);
			}

			Stulu::UUID receivedUUID = Controls::ReceiveDragDopAsset(AssetType::Unknown, true);
			if (receivedUUID != Stulu::UUID::null) {
				MoveAsset(entry.path(), receivedUUID);
			}

			if (open) {
				if (hasChilds) {
					DrawDirectoryBrowser(entryPath);
				}

				ImGui::TreePop();
			}
		}
	}

	void AssetBrowser::DrawMenu() {
	}

	void AssetBrowser::DrawCreateFileModal() {

	}

	void AssetBrowser::DrawPopUps() {
		// general popups



		// asset popups
		if (!m_selectedPopupAsset)
			return;

		Asset& asset = *m_selectedPopupAsset;
		if (ImGui::BeginPopupModal("Delete Asset", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Delete this file? You will not be able to restore it!");
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				DeletePath(asset.path);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal("Rename Asset", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputText("##RENAME_INPUT", &m_assetRenameString);

			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Rename")) {
				const auto name = std::filesystem::path(asset.path).parent_path() / m_assetRenameString;
				if (RenameAsset(name, asset)) {
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
	}

	FileInteractAction AssetBrowser::DrawFileFrame(const std::filesystem::path& path, bool selected) {
		const float descriptionHeight = m_iconSize / 2.0f;
		const ImVec2 iconSize(m_iconSize, m_iconSize);
		const ImVec2 descriptionSize(m_iconSize, descriptionHeight);
		const ImVec2 totalSize(m_iconSize, m_iconSize + descriptionHeight);

		const ImVec4 imageBgColor = ImGui::GetStyle().Colors[ImGuiCol_PopupBg];
		const ImVec4 descriptionBgColor = ImGui::GetStyle().Colors[ImGuiCol_TabHovered];
		const ImVec4 shadowsColor = ImGui::GetStyle().Colors[ImGuiCol_TitleBg];
			  ImVec4 tintColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		const ImVec2 padding = ImVec2(ImGui::GetStyle().FramePadding.x / 2.0f, ImGui::GetStyle().FramePadding.y);

		const bool isDirectory = std::filesystem::is_directory(path);
		const std::string fileName = path.stem().string();

		ImGui::PushID(path.string().c_str());

		const ImVec2 pos = ImGui::GetCursorPos();
		const ImVec2 screenPos = ImGui::GetCursorScreenPos();

		FileInteractAction action = FileInteractAction::None;

		if (ImGui::Selectable("##selector", selected, 0, totalSize)) {
			action = FileInteractAction::Pressed;
		}
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			action = FileInteractAction::Open;
		}
		else if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			action = FileInteractAction::RightClick;
		}

		// draggin
		const auto uuid = AssetsManager::getFromPath(path);
		if (uuid == Stulu::UUID::null) {
			AssetsManager::add(path.string());
		}

		auto& asset = AssetsManager::get(uuid);
		Controls::DragDropAsset(uuid, asset.type);
		
		if (asset.type == AssetType::Directory) {
			Stulu::UUID receivedUUID = Controls::ReceiveDragDopAsset(AssetType::Unknown, true);
			if (receivedUUID != Stulu::UUID::null) {
				MoveAsset(asset.path, receivedUUID);
			}
			// directory icon is colored
			tintColor = ImVec4(1, 1, 1, 1);
		}
		
		// menu
		if (ImGui::BeginPopupContextItem("ASSET_CONTEXT_MENU")) {
			if (ImGui::MenuItem("Rename")) {
				m_assetRenameString = std::filesystem::path(asset.path).filename().string();
				ImGui::OpenPopup("Rename Asset");
			}
			if (ImGui::MenuItem("Delete")) {
				ImGui::OpenPopup("Delete Asset");
			}
			ImGui::EndPopup();
			m_selectedPopupAsset = &asset;
		}

		// bg
		if (!isDirectory) {
			// shadows
			ImGui::GetWindowDrawList()->AddRectFilled(screenPos, screenPos + totalSize + ImVec2(1, 1), ImGui::GetColorU32(shadowsColor), m_rounding);
			// rect
			ImGui::GetWindowDrawList()->AddRectFilled(screenPos, screenPos + totalSize, ImGui::GetColorU32(descriptionBgColor), m_rounding);
			// image bg
			ImGui::GetWindowDrawList()->AddRectFilled(screenPos, screenPos + iconSize, ImGui::GetColorU32(imageBgColor), 0.0f);
		}

		// icon
		const ImVec2 offset = iconSize * (1.0f / 8.0f);
		const ImVec2 imageSize = iconSize - offset;

		ImGui::SetCursorPos(pos + (offset / 2.0f));
		ImGui::Image(GetIcon(asset, path.extension().string()), imageSize, { 0,1 }, { 1,0 }, tintColor, { 0,0,0,0 });
		
		// header
		ImVec2 namePos = pos + ImVec2(0.0f, iconSize.y) + ImVec2(padding.x, padding.y);
		ImGui::SetCursorPos(namePos);

		if (isDirectory) {
			// centerText
			const float availSpace = descriptionSize.x - 2.0f * padding.x;
			const float nameSize = ImGui::CalcTextSize(fileName.c_str(), 0, false, availSpace).x + 10.0f;
			const float text_indentation = ((availSpace - nameSize) * 0.5f);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + text_indentation);
			ImGui::PushTextWrapPos(pos.x + descriptionSize.x - text_indentation);
		}
		else {
			ImGui::PushTextWrapPos(pos.x + descriptionSize.x);
		}

		ImGui::TextWrapped(fileName.c_str());
		ImGui::PopTextWrapPos();


		// type
		if (!isDirectory) {
			ImGui::PushFontSmall();

			std::string tag = magic_enum::enum_name<AssetType>(asset.type).data();
			std::transform(tag.begin(), tag.end(), tag.begin(), ::toupper);

			ImVec2 typeSize = ImGui::CalcTextSize(tag.c_str());
			ImGui::SetCursorPos(pos + totalSize - ImVec2(typeSize) - padding);
			ImGui::TextDisabled(tag.c_str());

			ImGui::PopFont();
		}
		ImGui::PopID();

		// dummy for adding item size to cursor pos and cursor_prev_line so SameLine will work again
		ImGui::SetCursorPos(pos);
		ImGui::Dummy(totalSize);



		return action;
	}

	void AssetBrowser::DeletePath(const std::filesystem::path& path) {
		
	}

	void AssetBrowser::DeleteAsset(const Stulu::UUID& uuid) {

	}

	bool AssetBrowser::DirectoryContainsSub(const std::filesystem::path& path, bool onlyDirs) {
		for (auto& entry : std::filesystem::directory_iterator(path)) {
			if (onlyDirs) {
				if (entry.is_directory())
					return true;
			}
			else {
				return true;
			}
		}
		return false;
	}

	bool AssetBrowser::MoveAsset(const std::filesystem::path& targetDir, Stulu::UUID& receivedUUID) {
		Stulu::Asset& received = AssetsManager::get(receivedUUID);
		const auto newPath = targetDir / std::filesystem::path(received.path).filename();
		return RenameAsset(newPath, received);
	}

	bool AssetBrowser::RenameAsset(const std::filesystem::path& targetName, Stulu::Asset& asset) {
		const auto newPathMeta = std::filesystem::path(targetName.string() + ".meta");
		const auto oldPathMeta = std::filesystem::path(asset.path + ".meta");

		if (std::filesystem::exists(targetName)) {
			ST_ERROR("Cannot override: {0}", targetName);
			return false;
		}
		if (std::filesystem::exists(newPathMeta)) {
			ST_ERROR("Cannot override: {0}", newPathMeta);
			return false;
		}

		if (!std::filesystem::exists(oldPathMeta)) {
			ST_ERROR("Cannot find: {0}", oldPathMeta);
			return false;
		}
		if (!std::filesystem::exists(asset.path)) {
			ST_ERROR("Cannot find: {0}", asset.path);
			return false;
		}

		std::filesystem::rename(asset.path, targetName);
		std::filesystem::rename(oldPathMeta, newPathMeta);

		asset.path = targetName.string();

		return true;
	}

	Ref<Texture> AssetBrowser::GetIcon(Asset& asset, const std::string& extension) {
		switch (asset.type)
		{
		case Stulu::AssetType::Texture2D:
			return std::any_cast<Ref<Texture>&>(asset.data);
		case Stulu::AssetType::RenderTexture:
			return std::any_cast<Ref<Texture>&>(asset.data);
		case Stulu::AssetType::SkyBox:
			return m_preview.GetSkyboxPreview(std::dynamic_pointer_cast<SkyBox>(std::any_cast<Ref<Texture>>(asset.data)));
		case Stulu::AssetType::Model:
			return m_preview.GetModelPreview(std::any_cast<Model&>(asset.data));
		case Stulu::AssetType::Mesh:
			return m_preview.GetMeshPreview(std::any_cast<MeshAsset&>(asset.data));
		case Stulu::AssetType::Material:
			return m_preview.GetMaterialPreview(std::any_cast<Ref<Material>>(asset.data));
		case Stulu::AssetType::Directory:
			return Resources::GetDirectoryIcon();
		case Stulu::AssetType::Scene:
			return Resources::GetSceneIcon();
		}

		return Resources::GetCodeIcon(extension);
	}
}