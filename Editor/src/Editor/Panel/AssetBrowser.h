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
	private:
		std::filesystem::path m_path;
		bool m_inspector = true;
		Asset selected = { AssetType::Unknown,"","",UUID::null };

		void renderInspector();
		void drawDirectory(const std::filesystem::path& path, bool includePathDir = false);
		bool containsDirSubDirs(std::filesystem::path path);

	};
}