#pragma once
#include <Stulu.h>

#include "Compiler.h"

namespace Editor {
	class Project {
	public:
		Project(const std::string& folder);

		void Compile(const BuildSettings& settings);
		std::string GetBinariesPath(BuildConfiguration settings = StandartBuildConfig) const;

		inline const std::string GetPath() const { return m_path; }
		inline const std::string GetName() const { return m_name; }
		inline const std::string GetAssetPath() const { return m_path + "/Assets"; }
		inline const std::string GetConfigPath() const { return m_path + "/Config"; }
		inline const std::string GetWindowINI() const { return GetConfigPath() + "/WindowLayout.ini"; }
		inline const std::string GetCachePath() const { return m_path + "/Compiler"; }
	private:
		std::string m_path;
		std::string m_name;

		PremakeCompiler m_premakeCompiler;
		EngineCompiler m_engineCompiler;
	};
}