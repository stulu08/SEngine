#include "st_pch.h"
#include "OrthographicCameraController.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"

namespace Stulu {
	OrthographicCameraController::OrthographicCameraController(float aspectRatio,bool zoom, bool move, bool rotation)
		:m_aspectRatio(aspectRatio), m_cam(-m_aspectRatio*m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 100.0f), m_zoom(zoom), m_moveVertical(move), m_moveHorizontal(move), m_rotation(rotation) {

	}
	OrthographicCameraController::OrthographicCameraController(float aspectRatio,bool zoom, bool moveVerical, bool m_moveHorizontal, bool rotation)
		:m_aspectRatio(aspectRatio), m_cam(-m_aspectRatio*m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 100.0f), m_zoom(zoom), m_moveVertical(moveVerical), m_moveHorizontal(m_moveHorizontal), m_rotation(rotation) {

	}
	void OrthographicCameraController::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		if (m_moveVertical) {
			if (Input::isKeyDown(KEY_W)) {
				m_transform.position.x += -sin(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
				m_transform.position.y += cos(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
			}
			if (Input::isKeyDown(KEY_S)) {
				m_transform.position.x -= -sin(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
				m_transform.position.y -= cos(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
			}
		}
		if(m_moveHorizontal) {
			if (Input::isKeyDown(KEY_A)) {
				m_transform.position.x -= cos(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
				m_transform.position.y -= sin(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
			}
			if (Input::isKeyDown(KEY_D)) {
				m_transform.position.x += cos(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
				m_transform.position.y += sin(glm::radians(m_transform.rotation.z)) * m_cameraMoveSpeed * timestep;
			}
		}
		m_cam.setPosition(m_transform.position);
		if (m_rotation) {
			if (Input::isKeyDown(KEY_LEFT))
				m_transform.rotation.z += m_cameraRotationSpeed * timestep * m_zoomLevel;
			if (Input::isKeyDown(KEY_RIGHT))
				m_transform.rotation.z -= m_cameraRotationSpeed * timestep * m_zoomLevel;

		}
		m_cam.setRotation(m_transform.rotation);
	}
	void OrthographicCameraController::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(ST_BIND_EVENT_FN(OrthographicCameraController::onMouseScroll));
		dispatcher.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(OrthographicCameraController::onResizeEvent));
	}
	void OrthographicCameraController::onResize(float width, float height) {
		ST_PROFILING_FUNCTION();
		m_aspectRatio = width / height;
		m_cam.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 100.0f);
		m_screenSize = glm::vec2(width , height);
	}
	bool OrthographicCameraController::onMouseScroll(MouseScrollEvent& e) {
		ST_PROFILING_FUNCTION();
		if (m_zoom) {
			m_zoomLevel -= e.getYOff() * .25f;
			m_zoomLevel = std::clamp(m_zoomLevel, minZoom, maxZoom);
			m_cam.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 100.0f);
		}
		return false;
	}
	bool OrthographicCameraController::onResizeEvent(WindowResizeEvent& e) {
		ST_PROFILING_FUNCTION();
		onResize((float)e.getWidth() , (float)e.getHeight());
		return false;
	}
}