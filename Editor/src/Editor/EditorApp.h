#include <Stulu.h>
#include "EditorLayer.h"

namespace Stulu {
	struct Project {
		std::string path;
		std::string assetPath;

		Project()
			:path(""),assetPath(path) {
		}
		Project(std::string& path)
#ifdef ST_PLATFORM_WINDOWS
			:path(path), assetPath(path + "\\assets") {
#else
			:path(path), assetPath(path + "/assets") {
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