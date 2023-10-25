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
		std::string* _path = (std::string*)par;
		if (!_path) {
			return ERROR_PATH;
		}
		std::string path = *_path;
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

		FILE* file = fopen("build.bat", "w+");
		if (!file) {
			CORE_ERROR("Building failed");
			return ERROR_BUILD;
		}
		arg = R"(
FOR /F "tokens=*" %%g IN ('"Engine\Editor\LooseFiles\tools\vswhere.exe" -latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (SET VAR=%%g)
"%VAR%" Stulu.sln /p:Configuration=Dist -verbosity:minimal -maxcpucount:)";
		arg += std::to_string(std::thread::hardware_concurrency());
		arg += " -nologo";
		fputs(arg.c_str(), file);
		fclose(file);

		if (system("call build.bat")) {
			CORE_ERROR("Building failed");
			return ERROR_BUILD;
		}

		return 0;
	}

	void StartDownload(const std::string& path) {
		if (Downloading) {
			CORE_ERROR("Already downloading")
			return;
		}
		if(!std::filesystem::exists(path))
			std::filesystem::create_directories(path);
		std::filesystem::current_path(path);

		thread = CreateThread(nullptr, 0, gitClone, (LPVOID)&path, 0, nullptr);
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