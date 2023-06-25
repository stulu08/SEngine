#include <Stulu.h>
#include "EditorLayer.h"

namespace Stulu {
	struct Project {
		std::string path = "";
		std::string name = "A Project";
		std::string assetPath = "";
		std::string configPath = "";
		std::string dataPath = "";
		std::string windowINI;

		Ref<ScriptAssembly> assembly;

		Project()
			:windowINI(configPath+"/windowLayout.ini") {
		}
		Project(const std::string& file)
#ifdef ST_PLATFORM_WINDOWS
			:path(path), assetPath(path + "\\assets"), configPath(path + "\\config"), dataPath(path + "\\data"), windowINI(configPath + "\\windowLayout.ini") {
#else
			:path(path), assetPath(path + "/assets"), configPath(path + "/config"), dataPath(path + "/data"), windowINI(configPath + "/windowLayout.ini") {
#endif
			
			size_t lastS = path.find_last_of("/\\");
			lastS = lastS == std::string::npos ? 0 : lastS + 1;
			name = path.substr(lastS, path.size() - lastS);
		}
		Project(const std::string& file, bool loadFolderIfFail) {
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
	};

	class EditorApp : public Application {
	public:
		EditorApp();
		virtual ~EditorApp();

		inline static EditorApp& get() { return *s_instance; }
		inline static EditorLayer& getLayer() { return *get().editorLayer; }
		inline static Project& getProject() { return get().s_project; }
	private:
		EditorLayer* editorLayer;
		static EditorApp* s_instance;
		static Project s_project;
	};

	void rebuildAssembly();
	void buildAssembly(const std::string& m_assembly);
}