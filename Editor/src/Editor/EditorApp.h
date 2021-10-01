#include <Stulu.h>
#include "EditorLayer.h"
namespace Stulu {
	class EditorApp : public Application {
	public:
		EditorApp() 
		: Application("Stulu Editor") {

			getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-app-icon.png");
			s_instance = this;
			editorLayer = new EditorLayer();
			pushLayer(editorLayer);
		}
		~EditorApp() {
			popLayer(editorLayer);
			delete editorLayer;
		}

		void onLoadingFinish() {
		}
		inline static EditorApp& get() { return *s_instance; }

	private:
		EditorLayer* editorLayer;

		static EditorApp* s_instance;
	};
}