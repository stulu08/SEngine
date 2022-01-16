#include "st_pch.h"
#ifdef ST_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Application.h"


namespace Stulu {
	float Platform::getTime() {
		return (float)glfwGetTime();
	}
	std::string Platform::openFile(const char* filter, const char* initialDir) {
		OPENFILENAMEA ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		//HWND hwnd;              // owner window
		//HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::get().getWindow().getNativeWindow());;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		//ofn.lpstrFileTitle = NULL;
		//ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = initialDir;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		// Display the Open dialog box. 
		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;
		
		return std::string();
	}
	std::string Platform::saveFile(const char* filter, const char* initialDir){
		OPENFILENAMEA ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		//HWND hwnd;              // owner window
		//HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::get().getWindow().getNativeWindow());
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		//ofn.lpstrFileTitle = NULL;
		//ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = initialDir;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

		// Display the Open dialog box. 

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}
	std::string Platform::browseFolder() {
		char szTitle[MAX_PATH];
		BROWSEINFOA bi;
		bi.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::get().getWindow().getNativeWindow());
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szTitle;
		bi.lpszTitle = "Select a folder";
		bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
		bi.lpfn = NULL;
		bi.lParam = 0;
		LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
		if (pidl != NULL) {
			if (SHGetPathFromIDListA(pidl, szTitle))
				return szTitle;
		}

		return std::string(); /*
		
		OPENFILENAMEA ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		//HWND hwnd;              // owner window
		//HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::get().getWindow().getNativeWindow());;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFileTitle = "select.this.directory";
		ofn.nMaxFileTitle = 21;
		ofn.lpstrFilter = "Directory\0*.this.directory";
		ofn.nFilterIndex = 1;
		//ofn.lpstrFileTitle = NULL;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		// Display the Open dialog box. 
		if (GetSaveFileNameA(&ofn) == TRUE) {
			size_t pos = std::string(ofn.lpstrFile).find_last_of("\\/");
			return (std::string::npos == pos)
				? ""
				: std::string(ofn.lpstrFile).substr(0, pos);
		}

		return std::string();*/
	}
	bool Platform::createDirectory(const char* directory) {	
		if (CreateDirectoryA(std::string(directory).c_str(), NULL)) {
			return true;
		}
		CORE_ERROR("Cant create directory({0}): {1}", std::to_string(GetLastError()), std::string(directory));
		return false;
	}
	//https://stackoverflow.com/a/64166/13917861
	MemoryUsageInfo Platform::getMemoryUsage() {
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		uint64_t totalVirtualMem = memInfo.ullTotalPageFile;
		uint64_t virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		uint64_t virtualMemUsedByMe = pmc.PrivateUsage;

		uint64_t totalPhysMem = memInfo.ullTotalPhys;
		uint64_t physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
		uint64_t physMemUsedByMe = pmc.WorkingSetSize;

		return { totalVirtualMem,virtualMemUsed,virtualMemUsedByMe,totalPhysMem,physMemUsed,physMemUsedByMe };
	}

}
#endif // PLAFORM_WINDOWS