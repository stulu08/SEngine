#pragma once
#include <Stulu.h>
//Splash Screen
class Intro : public Stulu::Layer {
public:
	Intro(std::function<void(Stulu::Application*)> callback, Stulu::Application* app, float duration = 3.0f)
		: Layer("StuluEngine Intro"), dura(duration/2.0f), onFinish(callback), cam(1280.0f / 720.0f), app(app){
		cam.setZoomLevel(cam.minZoom);

		texture = Stulu::Texture2D::create("Stulu/assets/Textures/Logo/engine-logo-white.png");
	}

	virtual void onUpdate(Stulu::Timestep t) override{
		if (finish)
			return;

		cam.onUpdate(t);

		Stulu::RenderCommand::clear();
		Stulu::Renderer2D::beginScene(cam.getCamera());

		if (passedHalf) {
			Stulu::Renderer2D::drawTexturedQuad(texture, glm::vec2(.0f), glm::lerp(scale, scaleEnd, (cDura + dura) / (dura*2)), glm::vec2(1.0f), glm::lerp(color, colorTransp, cDura / dura));
		}
		else {
			Stulu::Renderer2D::drawTexturedQuad(texture, glm::vec2(.0f), glm::lerp(scale, glm::vec2(1.0f), cDura / dura), glm::vec2(1.0f), glm::lerp(colorTransp, color, cDura / dura));
		}
		Stulu::Renderer2D::endScene();

		if (cDura > dura && passedHalf) {
			finish = true;
			onFinish(app);
			return;
		}
		

		if (!passedHalf && cDura > dura) {
			passedHalf = true;
			cDura = .0f;
		}
		cDura += t;
		finish = false;
	}
	virtual void onEvent(Stulu::Event& e) override {
		cam.onEvent(e);
	}
	bool finish = false;
private:

	Stulu::OrthographicCameraController cam;
	Stulu::Ref<Stulu::Texture2D> texture;
	float cDura = .0f;
	float dura = .0f;
	bool passedHalf = false;
	glm::vec4 color = COLOR_WHITE;
	glm::vec4 colorTransp = glm::vec4(1.0f,1.0f,1.0f,.0f);
	glm::vec2 scale = glm::vec2(.95f);
	glm::vec2 scaleEnd = glm::vec2(1.05f);
	Stulu::Application* app;
	std::function<void(Stulu::Application*)> onFinish;
};