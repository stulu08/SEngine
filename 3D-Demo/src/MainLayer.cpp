#include "MainLayer.h"

void MainLayer::onAttach() {

}

void MainLayer::onUpdate(Timestep time) {
	m_cam.onUpdate(time);
	m_world.update();

	RenderCommand::clear();

	Renderer2D::beginScene(m_cam.getCamera());

	m_world.draw();

	Renderer2D::endScene();
	
}

void MainLayer::onEvent(Event& e) {
	m_cam.onEvent(e);
}
