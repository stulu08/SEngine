#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Scene/Components.h"
#include "Stulu/Core/Application.h"

namespace Stulu {
	class OrthographicCameraController {
	public:
		OrthographicCameraController(float aspectRatio, bool zoom = false, bool move = false, bool rotation = false);
		OrthographicCameraController(float aspectRatio, bool zoom, bool moveVerical, bool m_moveHorizontal, bool rotation);

		void onUpdate(Timestep timestep);
		void onEvent(Event& e);
		void onResize(float width, float height);

		const float getAspectRatio() const{
			return m_aspectRatio;
		}

		OrthographicCamera& getCamera() { return m_cam; }
		const OrthographicCamera& getCamera() const { return m_cam; }

		glm::vec2 getScreenSize() { return m_screenSize; }

		Transform& getTransform() { return m_transform; }
		void setTransform(const Transform& transform) { m_transform = transform; }

		void setZoomLevel(float level) {
			ST_PROFILING_FUNCTION();
			m_zoomLevel = level;
			m_cam.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 100.0f);
		}
		void setMinZoomLevel(float level) {
			ST_PROFILING_FUNCTION();
			minZoom = level;
			m_zoomLevel = std::clamp(m_zoomLevel, minZoom, maxZoom);
			m_cam.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 100.0f);
		}
		void setMaxZoomLevel(float level) {
			ST_PROFILING_FUNCTION();
			maxZoom = level;
			m_zoomLevel = std::clamp(m_zoomLevel, minZoom, maxZoom);
			m_cam.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel, -1.0f, 100.0f);
		}
	private:
		float m_aspectRatio, m_zoomLevel = 1.0f, m_cameraMoveSpeed = 8.0f, m_cameraRotationSpeed = 64.0f;
		float maxZoom = 9.0f, minZoom = .5f;
		bool m_zoom, m_moveVertical, m_moveHorizontal, m_rotation;

		Transform m_transform;

		glm::vec2 m_screenSize = glm::vec2(Stulu::Application::get().getWindow().getWidth(), Stulu::Application::get().getWindow().getHeight());

		OrthographicCamera m_cam;

		bool onMouseScroll(MouseScrollEvent& e);
		bool onResizeEvent(WindowResizeEvent& e);
	};
}