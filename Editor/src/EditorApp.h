#include <Stulu.h>
#include "EditorLayer.h"

class EditorApp : public Stulu::Application {
public:
	EditorApp() {
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