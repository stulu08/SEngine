#include "Project.h"

namespace Editor {
	Project::Project(const std::string& folder)
		: m_path(folder), m_premakeCompiler(m_path), m_engineCompiler(m_path, GetCachePath()) {

		size_t lastS = m_path.find_last_of("/\\");
		lastS = lastS == std::string::npos ? 0 : lastS + 1;
		m_name = m_path.substr(lastS, m_path.size() - lastS);
	}
	void Project::Compile(const BuildSettings& settings) {
		if (!m_premakeCompiler.GenerateBuildFiles()) {
			return;
		}
		m_engineCompiler.CompileAssemblies(settings);
	}

	std::string Project::GetBinariesPath(BuildConfiguration config) const {
		std::string assemblyFolder = m_path + "/Data/";
		switch (config)
		{
		case BuildConfiguration::Debug:
			assemblyFolder += "Debug";
			break;
		case BuildConfiguration::Release:
			assemblyFolder += "Release";
			break;
		case BuildConfiguration::Dist:
			assemblyFolder += "Dist";
			break;
		}
#ifdef ST_PLATFORM_WINDOWS
		assemblyFolder += "-windows-x86_64";
#endif
		return assemblyFolder;
	}
}