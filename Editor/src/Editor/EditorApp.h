#include <Stulu.h>
#include "EditorLayer.h"
namespace Stulu {
	class EditorApp : public Application {
	public:
		EditorApp();
		virtual ~EditorApp();

		inline static EditorApp& get() { return *s_instance; }
		inline static EditorLayer& getLayer() { return *get().editorLayer; }
	private:
		EditorLayer* editorLayer;
		static EditorApp* s_instance;
	};
}