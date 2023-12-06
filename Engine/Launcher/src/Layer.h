#pragma once
#include <Stulu.h>

namespace Stulu::Launcher {
	struct BuildOptions {
		std::string path;
		bool debugBuild;
		bool buildUsingAllCores;
	};
#define ERROR_PATH 1
#define ERROR_GIT 2
#define ERROR_SETUP 3
#define ERROR_PREMAKE 4
#define ERROR_BUILD 5
#define ERROR_BUILD_FILE 6

	void StartDownload(const BuildOptions& path);
	bool CheckDownloadFinished(uint32_t& ret);
	std::string TranslateDownloadError(uint32_t code);

	class LauncherLayer : public Layer {
	public:
		LauncherLayer();
		~LauncherLayer();

		void onAttach() override;
		void onImguiRender(Timestep timestep) override;

		void drawDownloadWindow();
		void drawEngineWindow();
		void drawProjectsWindow();
		void drawHomeWindow();
		void drawMainWindow();
	private:
		bool m_showHome = true;
		bool m_showProjects = false;
		bool m_showEngine = false;
		bool m_openDownload = false;
		bool m_downloading = false;

		float m_width = 0.0f;
		float m_height = 0.0f;
		float m_userHeight = 0.0f;
		float m_menuBarWidth = 0.0f;
		float m_contentAreaWidth = 0.0f;
	};
}