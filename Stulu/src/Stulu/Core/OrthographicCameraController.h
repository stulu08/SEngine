#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/MouseEvent.h"
namespace Stulu {
	class OrthographicCameraController {
	public:
		OrthographicCameraController(float aspectRatio, bool zoom = false, bool move = false, bool rotation = false);
		OrthographicCameraController(float aspectRatio, bool zoom, bool moveVerical, bool m_moveHorizontal, bool rotation);

		void onUpdate(Timestep timestep);
		void onEvent(Event& e);

		const float getAspectRatio() const{
			return m_aspectRatio;
		}

		OrthographicCamera& getCamera() { return m_cam; }
		const OrthographicCamera& getCamera() const { return m_cam; }

		glm::vec2 getScreenSize() { return m_screenSize; }
	private:
		float m_aspectRatio, m_zoomLevel = 1.0f, m_cameraMoveSpeed = 8.0f, m_cameraRotationSpeed = 64.0f, m_cameraRotation = 0.0f;
		bool m_zoom, m_moveVertical, m_moveHorizontal, m_rotation;

		glm::vec3 m_cameraPos = {0.0f,0.0f,0.0f};
		glm::vec2 m_screenSize = glm::vec2(1280.0f,720.0f);

		OrthographicCamera m_cam;

		bool onMouseScroll(MouseScrollEvent& e);
		bool onResizeEvent(WindowResizeEvent& e);
	};
}