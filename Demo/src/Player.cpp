#include "Player.h"
#include "Game.h"

namespace Demo {
	void Player::onStart() {
		gameObject.getComponent<GameObjectBaseComponent>().tag = "MainCam";
		gameObject.addComponent<CameraComponent>().settings.clearType = CameraComponent::Skybox;
		gameObject.addComponent<SkyBoxComponent>().texture = std::any_cast<Ref<CubeMap>>(AssetsManager::get(Stulu::UUID(11)).data);
	}
	void Player::onUpdate() {
		auto& transform = getComponent<TransformComponent>();
		float speed = this->speed;
		if (Input::isKeyDown(Keyboard::LeftShift))
			speed *= sprintMultiply;
		if (Input::isKeyDown(Keyboard::W))
			transform.position += transform.forwardDirection() * speed * Time::deltaTime;
		if (Input::isKeyDown(Keyboard::S))
			transform.position += -transform.forwardDirection() * speed * Time::deltaTime;
		if (Input::isKeyDown(Keyboard::A))
			transform.position += -transform.rightDirection() * speed * Time::deltaTime;
		if (Input::isKeyDown(Keyboard::D))
			transform.position += transform.rightDirection() * speed * Time::deltaTime;
		if (Input::isKeyDown(Keyboard::LeftControl))
			transform.position += -transform.upDirection() * speed * Time::deltaTime;
		if (Input::isKeyDown(Keyboard::Space))
			transform.position += transform.upDirection() * speed * Time::deltaTime;


		float horizontal = -Input::getMouseDelta().x * Time::deltaTime;
		float vertical = -Input::getMouseDelta().y * Time::deltaTime;
		float yawSign = transform.upDirection().y < 0 ? -1.0f : 1.0f;
		transform.rotation.y += yawSign * horizontal * sensitivity;
		transform.rotation.x += vertical * sensitivity;
	}
}