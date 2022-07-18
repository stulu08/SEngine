#include "st_pch.h"
#ifdef ST_PLATFORM_LINUX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "Stulu/Core/Platform.h"
#include "Stulu/Core/Application.h"
namespace Stulu {
	float Platform::getTime() {
		return (float)glfwGetTime();
	}
	std::string Platform::openFile(const char* filter, const char* initialDir) {
		ST_PROFILING_FUNCTION();
		char filename[1024];
		FILE* f = popen((std::string("zenity --file-selection --title='Select a File to open' --file-filter=\"") + filter + "\" ").c_str(), "r");
		fgets(filename, 1024, f);

		return filename;
	}
	std::string Platform::saveFile(const char* filter, const char* initialDir) {
		return openFile();
	}
	std::string Platform::browseFolder() {
		ST_PROFILING_FUNCTION();
		char filename[1024];
		FILE* f = popen((std::string("zenity --file-selection --title='Select a File to open' --file-filter=\"") + filter + "\" --directory").c_str(), "r");
		fgets(filename, 1024, f);

		return filename;
	}
	bool Platform::createDirectory(const char* directory) {
		ST_PROFILING_FUNCTION();
		const int dir_err = mkdir(directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
			CORE_ERROR("Cant create directory({0}): {1}", std::to_string(GetLastError()), std::string(directory));
			return false;
		}
		return true;
	}
	//https://stackoverflow.com/a/64166/13917861
	int ST_procParseLine(char* line) {
		ST_PROFILING_FUNCTION();
		// This assumes that a digit will be found and the line ends in " Kb".
		int i = strlen(line);
		const char* p = line;
		while (*p < '0' || *p > '9') p++;
		line[i - 3] = '\0';
		i = atoi(p);
		return i;
	}
	int ST_procmemgetValue(const char* name, size_t count) { //Note: this value is in KB!
		ST_PROFILING_FUNCTION();
		FILE* file = fopen("/proc/self/status", "r");
		int result = -1;
		char line[128];

		while (fgets(line, 128, file) != NULL) {
			if (strncmp(line, name, count) == 0) {
				result = ST_procParseLine(line);
				break;
			}
		}
		fclose(file);
		return result;
	}
	MemoryUsageInfo Platform::getMemoryUsage() {
		ST_PROFILING_FUNCTION();
		struct sysinfo memInfo;
		sysinfo(&memInfo);
		uint64_t totalVirtualMem = memInfo.totalram;
		totalVirtualMem += memInfo.totalswap;
		totalVirtualMem *= memInfo.mem_unit;

		uint64_t virtualMemUsed = memInfo.totalram - memInfo.freeram;
		virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
		virtualMemUsed *= memInfo.mem_unit;

		uint64_t virtualMemUsedByMe = ST_procmemgetValue("VmSize:",7) * 1024;

		uint64_t totalPhysMem = memInfo.totalram;
		totalPhysMem *= memInfo.mem_unit;
		uint64_t physMemUsed = memInfo.totalram - memInfo.freeram;
		physMemUsed *= memInfo.mem_unit;
		uint64_t physMemUsedByMe = ST_procmemgetValue("VmRSS:", 6) * 1024;

		return { totalVirtualMem,virtualMemUsed,virtualMemUsedByMe,totalPhysMem,physMemUsed,physMemUsedByMe };
	}
	bool Platform::changeWorkingDirectory(const char* dir = NULL) {
		int succes;
		succes = !chdir(dir);
		return succes;
	}
	const char* Platform::getCurrentWorkingDirectory() {
		char path[PATH_MAX];
		getcwd(path, PATH_MAX);
		return path;
	}
}
#endif // ST_PLATFORM_LINUX