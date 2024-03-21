#pragma once
#include <Stulu.h>
#include <StuluPremake.h>

#include "Stulu/Scene/YAML.h"

namespace Stulu {
	class Project {
	public:
		static inline Scope<Project> Main = nullptr;

		Project() {}
		Project(const std::string& folder);

		void generateProjectFiles();
		void compileAssembly();
		std::string createBuildFile();
		std::string getBinariesDir() const;

		std::string path = "";
		std::string name = "A Project";
		std::string assetPath = "";
		std::string configPath = "";
		std::string windowINI;

		struct BuildSettings{
			enum Config {
				Debug = 1,
				Release = 2,
				Dist = 3,
			};
#ifdef ST_DEBUG
			Config config = Config::Debug;
#elif defined(ST_RELEASE)
			Config config = Config::Release;
#else
			Config config = Config::Dist;
#endif
		}buildSettings;
	};
}