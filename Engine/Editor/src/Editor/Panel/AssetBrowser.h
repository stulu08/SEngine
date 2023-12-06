#pragma once
#include <Stulu.h>
#include <filesystem>

namespace Stulu {
	class AssetBrowserPanel {
	public:
		AssetBrowserPanel(const std::filesystem::path& path = "assets");
		void render(bool* open);

		const Ref<Texture>& getIcon(const std::filesystem::directory_entry& directory);

		void deletePath(const std::filesystem::directory_entry& directory);

		static inline Asset& GetSelected() { return selected; }
	private:
		std::filesystem::path m_path;
		static inline Asset selected = { AssetType::Unknown,"","",UUID::null };

		void drawCreateFilePopUp();
		void drawDirectory(const std::filesystem::path& path, bool includePathDir = false);
		bool containsDirSubDirs(std::filesystem::path path);

	};
}