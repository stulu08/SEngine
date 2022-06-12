#pragma once
#include <Stulu.h>
//Splash Screen
class Splash {
public:
	Splash(float duration = 3.0f)
		: dura(duration/2.0f){

		aspectRatio = (float)Stulu::Application::getWidth() / (float)Stulu::Application::getHeight();
		cam = Stulu::createRef<Stulu::OrthographicCamera>(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, .01f,100.0f);

		texture = Stulu::Texture2D::create("Engine/logo-white.png");
	}

	bool onUpdate(Stulu::Timestep t) {
		if (finish)
			return finish;

		static const glm::mat4 view = glm::inverse(Stulu::Math::createMat4(glm::vec3(.0f, .0f, -5.0f), glm::vec3(1.0f)));
		if (aspectRatio != Stulu::Application::getWidth() / Stulu::Application::getHeight()) {
			aspectRatio = Stulu::Application::getWidth() / Stulu::Application::getHeight();
			cam->setProjection(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel, .01f, 100.0f);
		}


		Stulu::RenderCommand::clear();
		Stulu::Renderer::beginScene(cam->getProjectionMatrix(), view);
		Stulu::Renderer2D::beginScene();

		if (passedHalf) {
			Stulu::Renderer2D::drawTexturedQuad(texture, glm::vec2(.0f), glm::lerp(scale, scaleEnd, (cDura + dura) / (dura*2)), glm::vec2(1.0f), glm::lerp(color, colorTransp, cDura / dura));
		}
		else {
			Stulu::Renderer2D::drawTexturedQuad(texture, glm::vec2(.0f), glm::lerp(scale, glm::vec2(1.0f), cDura / dura), glm::vec2(1.0f), glm::lerp(colorTransp, color, cDura / dura));
		}

		Stulu::Renderer2D::drawQuad(glm::vec2(.0f), glm::vec2(1.0f), COLOR_WHITE_VEC4);


		Stulu::RenderCommand::setCullMode(Stulu::CullMode::BackAndFront);
		Stulu::Renderer2D::endScene();
		Stulu::Renderer::endScene();

		if (cDura > dura && passedHalf) {
			finish = true;
			return finish;
		}
		

		if (!passedHalf && cDura > dura) {
			passedHalf = true;
			cDura = .0f;
		}
		cDura += t;
		finish = false;

		return finish;
	}
	bool finish = false;
private:
	Stulu::Ref<Stulu::Camera> cam;
	Stulu::Ref<Stulu::Texture2D> texture;
	float cDura = .0f;
	float dura = .0f;
	bool passedHalf = false;
	float aspectRatio;
	float zoomLevel = .5f;
	glm::vec4 color = COLOR_WHITE;
	glm::vec4 colorTransp = glm::vec4(1.0f,1.0f,1.0f,.0f);
	glm::vec2 scale = glm::vec2(.95f);
	glm::vec2 scaleEnd = glm::vec2(1.05f);
};
