#include <Stulu.h>
#ifdef ST_PLATFORM_WINDOWS

#include <filesystem>

#include "Layer.h"

#include <Windows.h>
#include <process.h>
#include <shellapi.h>

namespace Stulu::Launcher {
	static bool Downloading = false;
	HANDLE thread = NULL;

	DWORD WINAPI gitClone(LPVOID par) {
		BuildOptions* options = (BuildOptions*)par;
		if (!options) {
			return ERROR_PATH;
		}
		const std::string path = options->path;

		std::string arg = "git clone https://github.com/stulu08/SEngine.git ";
		arg += "\"" + path + "\" --recursive";
		if (std::system(arg.c_str())) {
			std::filesystem::current_path(path);
			if (std::system("git status")) {
				CORE_ERROR("git failed");
				return ERROR_GIT;
			}
			if (std::system("git pull")) {
				CORE_ERROR("git failed");
				return ERROR_GIT;
			}
		}

		std::filesystem::current_path(path + "\\scripts");
		if (system("python python/setup.py nobuild")) {
			CORE_ERROR("Setup failed");
			return ERROR_SETUP;
		}
		std::filesystem::current_path(path);
		if (system("call Dependencies\\premake\\premake5.exe vs2022")) {
			CORE_ERROR("Generating project files failed");
			return ERROR_PREMAKE;
		}

		std::filesystem::current_path(path);

		const char* configs[] ={
			"Debug","Release","Dist"
		};
		for (int i = (options->debugBuild ? 0 : 2); i < 3; i++) {
			std::ofstream fileStream;
			fileStream.open("build.bat", std::ios::out);

			if (!fileStream.is_open()) {
				CORE_ERROR("Building failed");
				return ERROR_BUILD;
			}

			fileStream << R"(FOR /F "tokens=*" %%g IN ('"Engine\Editor\LooseFiles\tools\vswhere.exe" )";
			fileStream << R"(-latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (SET VAR=%%g))" << std::endl;
			fileStream << R"("%VAR%" Stulu.sln /p:Configuration=)" << configs[i] << " -verbosity:minimal -maxcpucount:";
			if(options->buildUsingAllCores)
				fileStream << std::thread::hardware_concurrency();
			else
				fileStream << "1";
			fileStream << " -nologo";

			fileStream.close();

			if (system("call build.bat")) {
				CORE_ERROR("Building failed");
				return ERROR_BUILD;
			}
		}

		return 0;
	}

	void StartDownload(const BuildOptions& options) {
		if (Downloading) {
			CORE_ERROR("Already downloading")
			return;
		}
		if(!std::filesystem::exists(options.path))
			std::filesystem::create_directories(options.path);
		std::filesystem::current_path(options.path);

		thread = CreateThread(nullptr, 0, gitClone, (LPVOID)&options, 0, nullptr);
		if (!thread) {
			DWORD err = GetLastError();
			CORE_ERROR("CreateThread() failed with error {0}", err);
			return;
		}
		Downloading = true;
	}
	bool CheckDownloadFinished(uint32_t& ret) {
		if (!Downloading)
			return false;

		if (!thread) {
			Downloading = false;
			return false;
		}
		DWORD result = WaitForSingleObject(thread, 0);
		if (result == WAIT_OBJECT_0) {
			GetExitCodeThread(thread, (LPDWORD)&ret);
			CloseHandle(thread);
			thread = nullptr;
			return true;
		}
		return false;
	}
	std::string TranslateDownloadError(uint32_t code) {
		/*
#define ERROR_PATH 1
#define ERROR_GIT 2
#define ERROR_SETUP 3
#define ERROR_PREMAKE 4
#define ERROR_BUILD 5
#define ERROR_BUILD_FILE 6
		*/
		switch (code)
		{
		case ERROR_PATH:
			return "Invalid Path";
		case ERROR_GIT:
			return "Could not clone or pull repo";
		case ERROR_SETUP:
			return "Error while running setup";
		case ERROR_PREMAKE:
			return "Error while generating build files";
		case ERROR_BUILD_FILE:
		case ERROR_BUILD:
			return "Error while building";
		}
		return "Could not translate!";
	}
}
#endif