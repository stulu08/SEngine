#include <Stulu.h>
#include "EditorLayer.h"
namespace Stulu {
	class EditorApp : public Application {
	public:
		EditorApp();
		~EditorApp() = default;

		inline static EditorApp& get() { return *s_instance; }
	private:
		EditorLayer* editorLayer;

		static EditorApp* s_instance;
	};
}