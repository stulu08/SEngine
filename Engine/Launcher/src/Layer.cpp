#include "Layer.h"
#include "App.h"

#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Stulu::Launcher {

	LauncherLayer::LauncherLayer()
		: Layer("LauncherLayer") {
	}

	LauncherLayer::~LauncherLayer() {

	}

	void LauncherLayer::onAttach() {
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
		ImGui::StyleColorsOceanDark();
		m_logoTexture = Texture2D::create("Textures/PNG - Logo - White.png");
	}

	void LauncherLayer::onImguiRender(Timestep timestep) {
		RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		RenderCommand::clear();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);

		const float margin = ImGui::GetStyle().ItemSpacing.x;
		m_width = viewport->Size.x;
		m_height = viewport->Size.y;
		m_menuBarWidth = m_width / 8.0f;
		m_userHeight = m_height - (margin * 2.0f);
		m_contentAreaWidth = m_width - m_menuBarWidth - (margin * 3.0f);

		if (ImGui::Begin("##Main", (bool*)0, ImGuiWindowFlags_NoDecoration)) {
			if (ImGui::BeginChild("##Menu", ImVec2(m_menuBarWidth, m_userHeight), true)) {
				if (ImGui::Selectable("Home", &m_showHome, ImGuiSelectableFlags_SpanAllColumns)) {
					m_showEngine = m_showProjects = false;
					m_showHome = true;
				}
				if (ImGui::Selectable("Projects", &m_showProjects, ImGuiSelectableFlags_SpanAllColumns)) {
					m_showEngine = m_showHome = false;
					m_showProjects = true;
				}
				if (ImGui::Selectable("Engine", &m_showEngine, ImGuiSelectableFlags_SpanAllColumns)) {
					m_showHome = m_showProjects = false;
					m_showEngine = true;
				}
			}
			ImGui::EndChild();
			ImGui::SameLine();
			if (ImGui::BeginChild("##Content")) {
				drawMainWindow();
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}
	void LauncherLayer::drawDownloadWindow() {
		if (ImGui::BeginPopupModal("Download Engine", 0, ImGuiWindowFlags_AlwaysAutoResize)) {
			static BuildOptions options = { "C:\\Stulu\\Engine", false, true };

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2);
			ImGui::Text("Path");
			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);

			ImGui::SetNextItemWidth(m_contentAreaWidth / 3.0f);
			ImGui::InputText("##Path", &options.path);

			ImGui::SameLine();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::Button("Browse")) {
				options.path = Platform::browseFolder();
			}
			

			ImGui::Checkbox("Debug builds", &options.debugBuild);
			ImGui::SameLine();
			ImGui::HelpMarker("Size with debug builds: ~8GB\nSize without debug buils: ~4GB");

			ImGui::SameLine();
			ImGui::Checkbox("Fastbuild", &options.buildUsingAllCores);
			ImGui::SameLine();
			ImGui::HelpMarker(("Utilizes all concurrent threads(" + std::to_string(std::thread::hardware_concurrency()) + ") availabe for building").c_str());


			if (ImGui::Button("Download")) {
				StartDownload(options);
				m_downloading = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}


		uint32_t ret = 0;
		if (CheckDownloadFinished(ret)) {
			if (ret) {
				Platform::Message("Error", "An error occured while downloading: " + std::to_string(ret) + "\n" + TranslateDownloadError(ret), (uint32_t)MessageBoxType::Icon_Error | (uint32_t)MessageBoxType::Ok);
			}
			else {
				Platform::Message("Sucess", "Engine was downloaded", (uint32_t)MessageBoxType::Icon_Information | (uint32_t)MessageBoxType::Ok);
			}
			m_downloading = false;
		}
	}
	void LauncherLayer::drawEngineWindow() {
		if (m_downloading) {
			ImGui::BeginDisabled();
			const uint32_t t = (uint32_t)Time::applicationRuntime;
			if (t % 4 == 0) {
				ImGui::Button("Downloading");
			}
			else if (t % 4 == 1) {
				ImGui::Button("Downloading.");
			}
			else if (t % 4 == 2) {
				ImGui::Button("Downloading..");
			}
			else if (t % 4 == 3) {
				ImGui::Button("Downloading...");
			}
			ImGui::EndDisabled();
		}else if (ImGui::Button("Download Engine")) {
			m_openDownload = true;
		}

	}
	void LauncherLayer::drawProjectsWindow() {
		if (ImGui::Button("Add Existing")) {
			Platform::openFile("Stulu Project Files (*.sproj)\0 *.sproj");
		}
	}
	void LauncherLayer::drawHomeWindow() {
		const float aspect = (float)m_logoTexture->getHeight() / (float)m_logoTexture->getWidth();
		const float width = m_contentAreaWidth / 2.0f;
		const float height = m_contentAreaWidth / 2.0f * aspect;
		ImGui::SetCursorPosX((ImGui::GetWindowSize().x - width) * 0.5f);
		ImGui::Image(m_logoTexture, { width, height }, { 0,1 }, { 1,0 });
	}
	void LauncherLayer::drawMainWindow() {
		if (m_showHome) {
			if (ImGui::BeginChild("Home", ImVec2(m_contentAreaWidth, m_userHeight), true)) {
				drawHomeWindow();
			}
			ImGui::EndChild();
		}
		if (m_showProjects) {
			if (ImGui::BeginChild("Projects", ImVec2(m_contentAreaWidth, m_userHeight), true)) {
				drawProjectsWindow();
			}
			ImGui::EndChild();
		}
		if (m_showEngine) {
			if (ImGui::BeginChild("Engine", ImVec2(m_contentAreaWidth, m_userHeight), true)) {
				drawEngineWindow();
			}
			ImGui::EndChild();
		}
		if (m_openDownload) {
			m_openDownload = false;
			ImGui::OpenPopup("Download Engine");
		}
		drawDownloadWindow();
	}
}