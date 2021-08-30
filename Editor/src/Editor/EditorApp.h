#include <Stulu.h>
#include "EditorLayer.h"
namespace Stulu {
#ifdef ST_DIST
	class Splash : public Layer {
	public:
		Splash(std::function<void(Application*)> callback, Application* app, float duration = 3.0f)
			: Layer("StuluEngine Intro"), dura(duration / 2.0f), onFinish(callback), cam(Application::get().getWindow().getWidth() / Application::get().getWindow().getHeight()), app(app) {
			cam.setZoomLevel(.5f);

			texture = Texture2D::create("Stulu/assets/Textures/Logo/engine-logo-white.png");
		}

		virtual void onUpdate(Timestep t) override {
			if (finish)
				return;

			cam.onUpdate(t);

			RenderCommand::clear();
			Renderer2D::beginScene(cam.getCamera());

			if (passedHalf) {
				Renderer2D::drawTexturedQuad(texture, glm::vec2(.0f), glm::lerp(scale, scaleEnd, (cDura + dura) / (dura * 2)), glm::vec2(1.0f), glm::lerp(color, colorTransp, cDura / dura));
			}
			else {
				Renderer2D::drawTexturedQuad(texture, glm::vec2(.0f), glm::lerp(scale, glm::vec2(1.0f), cDura / dura), glm::vec2(1.0f), glm::lerp(colorTransp, color, cDura / dura));
			}
			Renderer2D::endScene();

			if (cDura > dura && passedHalf) {
				finish = true;
				onFinish(app);
				app->popLayer(this);
				return;
			}


			if (!passedHalf && cDura > dura) {
				passedHalf = true;
				cDura = .0f;
			}
			cDura += t;
			finish = false;
		}
		virtual void onEvent(Event& e) override {
			cam.onEvent(e);
		}
		bool finish = false;
	private:

		OrthographicCameraController cam;
		Ref<Texture2D> texture;
		float cDura = .0f;
		float dura = .0f;
		bool passedHalf = false;
		glm::vec4 color = COLOR_WHITE;
		glm::vec4 colorTransp = glm::vec4(1.0f, 1.0f, 1.0f, .0f);
		glm::vec2 scale = glm::vec2(.95f);
		glm::vec2 scaleEnd = glm::vec2(1.05f);
		Application* app;
		std::function<void(Application*)> onFinish;
	};

#endif
	class EditorApp : public Application {
	public:
		EditorApp() 
		: Application("Stulu Editor") {

			getWindow().setWindowIcon("Stulu/assets/Textures/Logo/engine-white-icon.png");

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