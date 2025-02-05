#include "Compiler.h"

#include "App.h"

#include <Stulu/Core/Core.h>
#include <StuluPremake.h>
#include <filesystem>
#include <fstream>

namespace Editor {
	PremakeCompiler::PremakeCompiler(const std::string& target)
		: m_target(target) {}


	bool PremakeCompiler::GenerateBuildFiles() {
		// SEngine\Engine\Build\Release-windows-x86_64\Editor
		const auto& cPath = std::filesystem::absolute(std::filesystem::current_path());
		// SEngine
		const auto& instalPath = std::filesystem::absolute(cPath.string() + "/../../../../");
		if (!std::filesystem::exists(instalPath.string() + "/Dependencies")) {
			ST_CRITICAL("Not able to find engine installation directory! Found: {0}", instalPath);
			return false;
		}

		const std::string installDir = "--InstallDir=" + instalPath.string();
		const char* options[1];
		options[0] = installDir.c_str();

		int success = stulu_premake_exec_options(m_target.c_str(), ST_PREMAKE_ACTION_VS2022, options, 1);

		// had a bug only in debug mode, where the premake func didnt change the path back
		std::filesystem::current_path(cPath);
		return success == 0;
	}

	bool EngineCompiler::CompileAssemblies(const BuildSettings& settings) {
		ST_INFO("Compiling Project Assembly");

		std::string buildScript = GenerateBuildScript(settings);
		std::string buildCmd = "call " + buildScript;

		if (system(buildCmd.c_str())) {
			ST_ERROR("Compiling failed");
			return false;
		}
		ST_INFO("Compiling finished")
		return true;
	}

	std::string EngineCompiler::GenerateBuildScript(const BuildSettings& settings) {
		if (!std::filesystem::exists(m_cache))
			std::filesystem::create_directories(m_cache);

		const std::string file = m_cache + "/Build.bat";
		std::ofstream fileStream;
		fileStream.open(file, std::ios::out);

		if (!fileStream.is_open()) {
			ST_ASSERT(false, "Building failed");
			return "";
		}
		fileStream << "@echo off\n";
		// Data path for vswhere hard coded, execution possible before application init
		fileStream << R"(FOR /F "tokens=*" %%g IN ('"Data\vswhere\vswhere.exe" )";
		fileStream << R"(-latest -prerelease -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do (SET VAR=%%g))" << std::endl;
		fileStream << R"("%VAR%" )";
		fileStream << m_target << "/Assembly.sln /p:Configuration=";

		switch (settings.config)
		{
		case BuildConfiguration::Debug:
			fileStream << "Debug";
			break;
		case BuildConfiguration::Release:
			fileStream << "Release";
			break;
		case BuildConfiguration::Dist:
			fileStream << "Dist";
			break;
		}

		fileStream << " /target:";

		bool appendSemicolon = false;
		if (settings.targets & BuildTarget::Managed) {
			fileStream << "\"Managed Assembly\"";
			appendSemicolon = true;
		}
		if (settings.targets & BuildTarget::Native) {
			if (appendSemicolon)
				fileStream << ';';

			fileStream << "\"Native Assembly\"";
			appendSemicolon = true;
		}
		if (settings.targets & BuildTarget::Runtime) {
			if (appendSemicolon)
				fileStream << ';';

			fileStream << "\"Runtime\"";
		}

		fileStream << " -verbosity:minimal -maxcpucount:" << std::max(std::thread::hardware_concurrency() / 2u, 1u);
		fileStream << " -nologo";

		fileStream.close();

		return file;
	}
}
