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
	std::string Platform::openFile(const char* filter) {
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
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		// Display the Open dialog box. 

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}
	std::string Platform::saveFile(const char* filter){
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
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

		// Display the Open dialog box. 

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}
	
	bool Platform::createDirectory(const char* directory) {	
		if (CreateDirectoryA(std::string(directory).c_str(), NULL)) {
			return true;
		}
		CORE_ERROR("Cant create directory({0}): {1}", std::to_string(GetLastError()), std::string(directory));
		return false;
	}

}
#endif // PLAFORM_WINDOWS