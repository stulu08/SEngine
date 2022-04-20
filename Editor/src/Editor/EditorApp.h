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
		Project(const std::string& path)
#ifdef ST_PLATFORM_WINDOWS
			:path(path), assetPath(path + "\\assets"), configPath(path + "\\config"), dataPath(path + "\\data"), windowINI(configPath + "\\windowLayout.ini") {
#else
			:path(path), assetPath(path + "/assets"), configPath(path + "/config"), dataPath(path + "/data"), windowINI(configPath + "/windowLayout.ini") {
#endif
			size_t lastS = path.find_last_of("/\\");
			lastS = lastS == std::string::npos ? 0 : lastS + 1;
			name = path.substr(lastS, path.size() - lastS);
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