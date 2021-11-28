#include "st_pch.h"
#include "CameraController.h"
#include <Stulu/Core/Keycodes.h>
#include "Stulu/Core/Input.h"
#include <Stulu/Core/MouseButtonCodes.h>
#include "Stulu/Renderer/RenderCommand.h"

namespace Stulu {
	CameraController::CameraController(CamerControllerSetting* settings, CameraMode mode) 
		: m_settings(settings){
		if(mode == CameraMode::Orthographic)
			m_activeCamera = createRef<OrthographicCamera>(-m_aspectRatio * m_settings->zoom, m_aspectRatio * m_settings->zoom, -m_settings->zoom, m_settings->zoom, m_settings->zNear, m_settings->zFar);
		else if(mode == CameraMode::Perspective)
			m_activeCamera = createRef<PerspectiveCamera>(m_settings->fov, m_aspectRatio, m_settings->zNear, m_settings->zFar);
	}

	void CameraController::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		if (m_settings->m_controlls) {
			if (Input::isKeyDown(Keyboard::W))
				m_transform.position += m_transform.forwardDirection() * m_speed * (float)timestep;
			if (Input::isKeyDown(Keyboard::S))
				m_transform.position -= m_transform.forwardDirection() * m_speed * (float)timestep;;
			if (Input::isKeyDown(Keyboard::A))
				m_transform.position -= m_transform.rightDirection() * m_speed * (float)timestep;
			if (Input::isKeyDown(Keyboard::D))
				m_transform.position += m_transform.rightDirection() * m_speed * (float)timestep;

			if (Input::isKeyDown(Keyboard::LeftShift))
				m_speed += 5.0f * timestep;
			else
				m_speed = m_settings->cameraBaseMoveSpeed;

			if (Input::isMouseDown(MOUSE_BUTTON_2)) {
				float yawSign = m_transform.upDirection().y < 0 ? -1.0f : 1.0f;
				m_yaw += yawSign * mouseDelta.x * m_settings->cameraSensitivity;
				m_pitch += mouseDelta.y * m_settings->cameraSensitivity;
				m_transform.rotation = glm::vec3(-m_pitch, -m_yaw, .0f);
			}
		}

		mouseDelta = glm::vec2(Input::getMouseX() - lastMouseXPos, Input::getMouseY() - lastMouseYPos) * 0.003f;
		lastMouseXPos = Input::getMouseX();
		lastMouseYPos = Input::getMouseY();
	}
	void CameraController::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(CameraController::onResizeEvent));
		dispatcher.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(CameraController::onKeyPressed));
	}
	void CameraController::onResize(float width, float height) {
		ST_PROFILING_FUNCTION();
		m_aspectRatio = width / height;
		if(m_mode == CameraMode::Orthographic)
			m_activeCamera->setProjection(-m_aspectRatio * m_settings->zoom, m_aspectRatio * m_settings->zoom, -m_settings->zoom, m_settings->zoom, m_settings->zNear, m_settings->zFar);
		else if(m_mode == CameraMode::Perspective)
			m_activeCamera->setProjection(m_settings->fov, m_aspectRatio, m_settings->zNear, m_settings->zFar);
	}
	bool CameraController::onResizeEvent(WindowResizeEvent& e) {
		onResize((float)e.getWidth(), (float)e.getHeight());
		return false;
	}
	bool CameraController::onKeyPressed(KeyDownEvent& e) {
		ST_PROFILING_FUNCTION();
		if (e.getKeyCode() == m_settings->wireFrameSwitchKey) {
			m_settings->wireFrame = !m_settings->wireFrame;
			RenderCommand::setWireFrame(m_settings->wireFrame);
		}
		return false;
	}
}