#pragma once
#include <Stulu.h>
#include <filesystem>

namespace Stulu {
	class AssetBrowserPanel {
	public:
		AssetBrowserPanel(const std::filesystem::path& path = "assets");
		void render();
	private:
		std::filesystem::path m_path;
	};
}