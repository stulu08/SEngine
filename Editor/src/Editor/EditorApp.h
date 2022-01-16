#include <Stulu.h>
#include "EditorLayer.h"

namespace Stulu {
	struct Project {
		std::string path;
		std::string assetPath;
		std::string configPath;
		std::string windowINI;

		Project()
			:path(""),assetPath(path),configPath(path),windowINI(configPath+"/windowLayout.ini") {
		}
		Project(std::string& path)
#ifdef ST_PLATFORM_WINDOWS
			:path(path), assetPath(path + "\\assets"), configPath(path + "\\config"), windowINI(configPath + "\\windowLayout.ini") {
#else
			:path(path), assetPath(path + "/assets"), configPath(path + "/config"), windowINI(configPath + "/windowLayout.ini") {
#endif
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
}