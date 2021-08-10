#include "Player.h"

Player::Player()
	: m_cameraController(1280.0f / 720.0f) {

	m_transform.setRotation(glm::vec3(.0f,.0f, 270.0f));
	m_transform.setPos(glm::vec3(.0f, .5f, .0f));
	m_transform.setScale(glm::vec3(.2f));

	m_texture = Texture2D::create("assets/triangle.png");
}

void Player::update(Timestep ts) {

	if (Input::isKeyDown(KEY_SPACE)) {
		m_transform.getPos().y += m_force * ts;
		if(m_transform.getRotation().z < 315.0f)
			m_transform.getRotation().z += m_rotationChangeForce * ts;
	}
	else {
		m_transform.getPos().y += m_drag * ts; 
		if (m_transform.getRotation().z > 225.0f)
			m_transform.getRotation().z += m_rotationChangeDrag * ts;
	}
	m_transform.getPos().x += m_levelSpeed * ts;


	m_cameraController.getCamera().setPosition(glm::vec3(m_transform.getPos().x, m_transform.getPos().y,5.0f));
}


void Player::draw() {
	Renderer2D::drawTexture2DQuad(m_texture, m_transform);
}


