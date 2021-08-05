#include "st_pch.h"
#include "OrthographicCameraController.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"

namespace Stulu {
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		:m_aspectRatio(aspectRatio), m_cam(-m_aspectRatio*m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_rotation(rotation) {

	}
	void OrthographicCameraController::onUpdate(Timestep timestep) {

		if (Input::isKeyDown(KEY_W)) {
			m_camerPos.x += -sin(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
			m_camerPos.y += cos(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
		}
		if (Input::isKeyDown(KEY_S)) {
			m_camerPos.x -= -sin(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
			m_camerPos.y -= cos(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
		}

		if (Input::isKeyDown(KEY_A)) {
			m_camerPos.x -= cos(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
			m_camerPos.y -= sin(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
		}
		if (Input::isKeyDown(KEY_D)) {
			m_camerPos.x += cos(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
			m_camerPos.y += sin(glm::radians(m_cameraRotation)) * m_cameraMoveSpeed * timestep;
		}

		if (m_rotation) {
			if (Input::isKeyDown(KEY_LEFT))
				m_cameraRotation += m_cameraRotationSpeed * timestep * m_zoomLevel;
			if (Input::isKeyDown(KEY_RIGHT))
				m_cameraRotation -= m_cameraRotationSpeed * timestep * m_zoomLevel;

			m_cam.setRotation(m_cameraRotation);
		}
		m_cam.setPosition(m_camerPos);
	}
	void OrthographicCameraController::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(ST_BIND_EVENT_FN(OrthographicCameraController::onMouseScroll));
		dispatcher.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(OrthographicCameraController::onResizeEvent));
	}
	bool OrthographicCameraController::onMouseScroll(MouseScrollEvent& e) {
		m_zoomLevel -= e.getYOff() * .25f;
		m_zoomLevel = std::max(m_zoomLevel, .25f);
		m_cam.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}
	bool OrthographicCameraController::onResizeEvent(WindowResizeEvent& e) {
		m_aspectRatio = (float)e.getWidth() / (float)e.getHeight();
		m_cam.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}
}