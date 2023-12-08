#include "Project.h"

namespace Stulu {
	Project::Project(const std::string& file, bool loadFolderIfFail) {
		ST_PROFILING_FUNCTION();
		if (std::filesystem::exists(file) && !std::filesystem::is_directory(file)) {
			YAML::Node node = YAML::LoadFile(file);
			name = node["name"].as<std::string>();
			path = std::filesystem::absolute(std::filesystem::path(file).parent_path()).string();
			assetPath = std::filesystem::absolute(path + "/" + node["assetPath"].as<std::string>()).string();
			configPath = std::filesystem::absolute(path + "/" + node["configPath"].as<std::string>()).string();
			dataPath = std::filesystem::absolute(path + "/" + node["dataPath"].as<std::string>()).string();
			windowINI = std::filesystem::absolute(path + "/" + node["windowINI"].as<std::string>()).string();
		}
		else if (loadFolderIfFail) {
			if (std::filesystem::path(file).has_extension() && std::filesystem::path(file).has_parent_path()) {
				path = std::filesystem::path(file).parent_path().string();
			}
			else {
				path = file;
			}
			assetPath = path + "/assets";
			configPath = path + "/config";
			dataPath = path + "/data";
			windowINI = configPath + "/windowLayout.ini";
			size_t lastS = path.find_last_of("/\\");
			lastS = lastS == std::string::npos ? 0 : lastS + 1;
			name = path.substr(lastS, path.size() - lastS);
		}
		else {
			windowINI = configPath + "/windowLayout.ini";
		}
	}
	void Project::generateProjectFiles() {
		ST_PROFILING_FUNCTION();
		stulu_premake_exec(path.c_str(), ST_PREMAKE_ACTION_VS2022);
	}

	void Project::rebuildAssembly() {
		ST_PROFILING_FUNCTION();
		generateProjectFiles();
#ifdef ST_PLATFORM_WINDOWS
		static std::function<bool(const std::string&)> recompileFinished = [=](const std::string&)->bool {
			return std::filesystem::exists(dataPath + "/EditorProjectAssembly.dll");
			};
		static std::function<bool(const std::string&)> recompile = [=](const std::string&)->bool {
			return system(createBuildFile().c_str());
			};
		assembly->reload(recompileFinished, recompile);
#else
		//will soon switch to mono xbuild for linux
		CORE_ERROR("Building Projects is not supported currently on this Platform");
#endif
		assembly->getClasses().clear();
		MonoClass* componentClass = Application::get().getAssemblyManager()->getScriptCoreAssembly()->createClass("Stulu", "Component");
		assembly->loadAllClasses(componentClass);
	}
	void Project::buildAssembly(const std::string& m_assembly) {
		ST_PROFILING_FUNCTION();
		generateProjectFiles();
#ifdef ST_PLATFORM_WINDOWS

		if (std::filesystem::exists(m_assembly))
			std::filesystem::remove(m_assembly);

		CORE_INFO("Compiling {0}", m_assembly);
		if (system(createBuildFile().c_str())) {
			CORE_ERROR("Compiling failed: {0}", m_assembly);
		}
		while (true) {
			if (std::filesystem::exists(m_assembly)) {
				if (std::filesystem::exists(m_assembly)) {
					CORE_INFO("Compiling finished");
					break;
				}
				else {
					CORE_ERROR("File not found: {0}", m_assembly);
					CORE_ERROR("Compiling failed: {0}", m_assembly);
					break;
				}

			}
		}
#else
		//will soon switch to mono xbuild for linux
		CORE_ERROR("Building Projects is not supported currently on this Platform");
#endif
	}
	std::string Project::createBuildFile() {
		ST_PROFILING_FUNCTION();
		const std::string file = path + "/build.bat";
		std::ofstream fileStream;
		fileStream.open(file, std::ios::out);

		if (!fileStream.is_open()) {
			CORE_ASSERT(false, "Building failed");
			return "echo Build failed";
		}

		fileStream << R"(FOR /F "tokens=*" %%g IN ('"tools\vswhere.exe" )";
		fileStream << R"(-latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (SET VAR=%%g))" << std::endl;
		fileStream << R"("%VAR%" )";
		fileStream << path << "/Assembly.sln /p:Configuration=";

		if (buildSettings.debug) {
			fileStream << "Debug";
		}
		else {
			fileStream << "Release";
		}

		fileStream << " -verbosity:minimal -maxcpucount:" << buildSettings.threads;
		fileStream << " -nologo";

		fileStream.close();

		return "call " + file;
	}
}