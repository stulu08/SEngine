#include "Project.h"

namespace Stulu {
	Project::Project(const std::string& folder)
	:	path(folder), 
		assetPath(folder + "/Assets"),
		configPath(folder + "/Config"),
		windowINI(configPath + "/windowLayout.ini") {

		size_t lastS = path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		name = path.substr(lastS, path.size() - lastS);
	}
	void Project::generateProjectFiles() {
		ST_PROFILING_FUNCTION();
		const auto& cPath = std::filesystem::absolute(std::filesystem::current_path());
		const auto& instalPath = std::filesystem::absolute(cPath.string() + "/../../../../");
		const std::string installDir = "--InstallDir=" + instalPath.string();

		const char* options[1];
		options[0] = installDir.c_str();

		stulu_premake_exec_options(path.c_str(), ST_PREMAKE_ACTION_VS2022, options, 1);
	}

	void Project::compileAssembly() {
		generateProjectFiles();
		const std::string target = getBinariesDir() + "/ManagedAssembly.dll";

		if (std::filesystem::exists(target))
			std::filesystem::remove(target);

		CORE_INFO("Compiling {0}", target);
		if (system(createBuildFile().c_str())) {
			CORE_ERROR("Compiling failed: {0}", target);
		}
		if (std::filesystem::exists(target)) {
			if (std::filesystem::exists(target)) {
				CORE_INFO("Compiling finished");
			}
			else {
				CORE_ERROR("Compiling failed: {0}", target);
			}
		}
	}
	std::string Project::createBuildFile() {
		ST_PROFILING_FUNCTION();
		if (!std::filesystem::exists(path + "/Compiler"))
			std::filesystem::create_directories(path + "/Compiler");
		const std::string file = path + "/Compiler/Build.bat";
		std::ofstream fileStream;
		fileStream.open(file, std::ios::out);

		if (!fileStream.is_open()) {
			CORE_ASSERT(false, "Building failed");
			return "echo Build failed";
		}

		fileStream << R"(FOR /F "tokens=*" %%g IN ('"Data\vswhere\vswhere.exe" )";
		fileStream << R"(-latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (SET VAR=%%g))" << std::endl;
		fileStream << R"("%VAR%" )";
		fileStream << path << "/Assembly.sln /p:Configuration=";

		switch (buildSettings.config)
		{
		case BuildSettings::Debug:
				fileStream << "Debug";
				break;
		case BuildSettings::Release:
			fileStream << "Release";
			break;
		case BuildSettings::Dist:
			fileStream << "Dist";
			break;
		}
		fileStream << " /target:\"Managed Assembly\"";
		fileStream << " -verbosity:minimal -maxcpucount:" << std::max(std::thread::hardware_concurrency() / 2u, 1u);
		fileStream << " -nologo";

		fileStream.close();

		return "call " + file;
	}
	std::string Project::getBinariesDir() const {
#ifdef ST_PLATFORM_WINDOWS
		std::string assemblyFolder = path + "/Data/";
		switch (buildSettings.config)
		{
		case BuildSettings::Debug:
			assemblyFolder += "Debug";
			break;
		case BuildSettings::Release:
			assemblyFolder += "Release";
			break;
		case BuildSettings::Dist:
			assemblyFolder += "Dist";
			break;
		}
		assemblyFolder += "-windows-x86_64";
#endif
		return assemblyFolder;
	}
}