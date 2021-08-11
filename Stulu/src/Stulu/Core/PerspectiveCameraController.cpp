#include "st_pch.h"
#include "PerspectiveCameraController.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"
#include "Stulu/Renderer/RenderCommand.h"

namespace Stulu {
	PerspectiveCameraController::PerspectiveCameraController(float aspectRatio, float fov, float zNear, float zFar)
		:m_aspectRatio(aspectRatio), m_fov(fov), m_zNear(zNear), m_zFar(zFar), m_cam(m_fov, m_aspectRatio, m_zNear, m_zFar) {

	}
	float lastMouseXPos = 0;
	float lastMouseYPos = 0;
	glm::vec2 mouseDelta = glm::vec2(0.0f);
	void PerspectiveCameraController::onUpdate(Timestep timestep) {
		if (Input::isKeyDown(KEY_W))
			m_transform.getPos() += m_cam.getForwardDirection() * m_cameraMoveSpeed * (float)timestep;
		if (Input::isKeyDown(KEY_S))
			m_transform.getPos() -= m_cam.getForwardDirection() * m_cameraMoveSpeed * (float)timestep;;
		if (Input::isKeyDown(KEY_A))
			m_transform.getPos() -= m_cam.getRightDirection() * m_cameraMoveSpeed * (float)timestep;
		if (Input::isKeyDown(KEY_D))
			m_transform.getPos() += m_cam.getRightDirection() * m_cameraMoveSpeed * (float)timestep;
		if (Input::isMouseDown(MOUSE_BUTTON_2)) {
			float yawSign = m_cam.getUpDirection().y < 0 ? -1.0f : 1.0f;
			m_yaw += yawSign * mouseDelta.x * cameraSensitivity;
			m_pitch += mouseDelta.y * cameraSensitivity;
			m_transform.setRotation(-m_pitch, -m_yaw, .0f);
		}
		if (Input::isKeyDown(KEY_LEFT_SHIFT))
			m_cameraMoveSpeed += 5.0f * timestep;
		else
			m_cameraMoveSpeed = cameraBaseMoveSpeed;
		m_cam.setTransform(m_transform);

		mouseDelta = glm::vec2(Input::getMouseX() - lastMouseXPos, Input::getMouseY() - lastMouseYPos) * 0.003f;
		lastMouseXPos = Input::getMouseX();
		lastMouseYPos = Input::getMouseY();
		m_cam.recalculateViewMatrix();
	}
	void PerspectiveCameraController::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(PerspectiveCameraController::onResizeEvent));
		dispatcher.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(PerspectiveCameraController::onKeyPressed));
	}
	bool PerspectiveCameraController::onResizeEvent(WindowResizeEvent& e) {
		m_aspectRatio = (float)e.getWidth() / (float)e.getHeight();
		m_cam.setProjection(m_fov, m_aspectRatio, m_zNear, m_zFar);
		return false;
	}
	bool PerspectiveCameraController::onKeyPressed(KeyDownEvent& e) {
		if (e.getKeyCode() == wireFrameSwitchKey) {
			wireFrame = !wireFrame;
			RenderCommand::setWireFrame(wireFrame);
		}
		return false;
	}

}