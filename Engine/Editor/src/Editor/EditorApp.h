#include <Stulu.h>

#include "Project.h"
#include "EditorLayer.h"

namespace Stulu {

	class EditorApp : public Application {
	public:
		EditorApp(const ApplicationInfo& info);
		virtual ~EditorApp();

		inline static EditorApp& get() { return *s_instance; }
		inline static EditorLayer& getLayer() { return *get().editorLayer; }
		inline static std::string getDataPath() { return "Data/Stulu/Editor"; }
	private:
		EditorLayer* editorLayer;
		static EditorApp* s_instance;
	};
}