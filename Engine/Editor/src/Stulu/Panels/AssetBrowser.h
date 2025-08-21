#pragma once
#include <Stulu/Resources/AssetsManager.h>

#include "Stulu/Panel.h"
#include "Stulu/Previewing.h"

namespace Editor {

	class CreateFileOption {
	public:
		using ConstructFunction = std::function<CreateFileOption* ()>;

		virtual ~CreateFileOption() = default;

		virtual void Create(const std::filesystem::path& folder) = 0;
		virtual void DrawImGui() = 0;
	};

	enum class FileInteractAction {
		None = 0,
		LeftClick = 1,
		RightClick = 2,
		DoubleClicked = 3,

		Pressed = LeftClick,
		Open = DoubleClicked,
	};


	class AssetBrowser : public Panel {
	public:
		AssetBrowser(const std::filesystem::path& path);
		virtual ~AssetBrowser() = default;

		virtual void DrawImGui() override;
		virtual void DrawImGuizmo() override {}
		virtual void PreWindow() override;
		virtual void PostWindow() override;

		// for files
		void SetSelcted(const std::filesystem::path& path);
		// for files
		inline bool IsSelected(const std::filesystem::path& path) const {
			return m_selected == path;
		}
		// for files
		inline void ClearSelected() {
			m_selected = "";
		}
		inline auto GetSelcted() {
			return m_selected;
		}

		// for the selected directory
		inline bool IsSelectedPath(const std::filesystem::path& path) const {
			return m_path == path;
		}
		// for the selected directory
		inline void SetPath(const std::filesystem::path& path) {
			m_path = path;
		}

		inline auto& GetPreview() {
			return m_preview;
		}
	private:
		const std::filesystem::path m_root;
		std::filesystem::path m_path;
		std::filesystem::path m_selected = "";
		
		Preview m_preview;
		
		std::unordered_map<std::string, CreateFileOption::ConstructFunction> m_createFileOptions;
		CreateFileOption* m_currentCreateOption = nullptr;
		std::string m_search, m_assetRenameString;
		std::filesystem::path m_selectedPopupFile;

		float m_rounding = 4.0f;
		ImVec2 m_indent = { 6.0f, 7.0f };
		float m_iconSize = 100.0f;

		void DrawBrowser();
		void DrawDirectoryBrowser(const std::filesystem::path& path);
		void DrawMenu();
		void DrawCreateFileModal();
		void DrawPopUps();
		// returns true when double pressed
		FileInteractAction DrawFileFrame(const std::filesystem::path& path, bool selected = false);

		void DeletePath(const std::filesystem::path& path);
		void DeleteAsset(const Stulu::UUID& uuid);
		bool DirectoryContainsSub(const std::filesystem::path& path, bool onlyDirs = false);

		bool MoveAsset(const std::filesystem::path& targetDir, Stulu::UUID& receivedUUID);
		bool RenameAsset(const std::filesystem::path& targetName, Stulu::SharedAssetData* asset);

		Stulu::Texture2D* GetIcon(Stulu::SharedAssetData* asset, const std::string& extension);
	};
}