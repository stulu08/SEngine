#pragma once
#include <Stulu.h>
#include <StuluPremake.h>

#include "Stulu/Scene/YAML.h"

namespace Stulu {
	class Project {
	public:

		Project()
			:windowINI(configPath + "/windowLayout.ini") {
		}
		Project(const std::string& folder)
#ifdef ST_PLATFORM_WINDOWS
			:path(folder), assetPath(folder + "\\Assets"), 
			configPath(folder + "\\Config"), dataPath(folder + "\\Data"), 
			windowINI(configPath + "\\windowLayout.ini") {
#else
			:path(folder), assetPath(folder + "/Assets"), 
			configPath(folder + "/Config"), dataPath(folder + "/Data"), 
			windowINI(configPath + "/windowLayout.ini") {
#endif
			size_t lastS = path.find_last_of("/\\");
			lastS = lastS == std::string::npos ? 0 : lastS + 1;
			name = path.substr(lastS, path.size() - lastS);
		}
		Project(const std::string& file, bool loadFolderIfFail);

		void generateProjectFiles();
		void rebuildAssembly();
		void buildAssembly(const std::string & m_assembly);
		std::string createBuildFile();

		std::string path = "";
		std::string name = "A Project";
		std::string assetPath = "";
		std::string configPath = "";
		std::string dataPath = "";
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

		Ref<ScriptAssembly> assembly;
	};
}