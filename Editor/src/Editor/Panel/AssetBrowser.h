#pragma once
#include <Stulu.h>
#include <filesystem>

namespace Stulu {
	class AssetBrowserPanel {
	public:
		AssetBrowserPanel(const std::filesystem::path& path = "assets");
		void render(bool* open);


		const Ref<Texture>& getIcon(const std::filesystem::directory_entry& directory);
	private:
		std::filesystem::path m_path;

		void drawDirectory(const std::string& path);
	};
}