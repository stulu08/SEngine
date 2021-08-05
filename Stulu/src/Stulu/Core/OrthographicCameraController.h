#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/MouseEvent.h"
namespace Stulu {
	class OrthographicCameraController {
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep timestep);
		void onEvent(Event& e);

		OrthographicCamera& getCamera() { return m_cam; }
		const OrthographicCamera& getCamera() const { return m_cam; }
	private:
		float m_aspectRatio;
		float m_zoomLevel = 1.0f;
		bool m_rotation;

		OrthographicCamera m_cam;

		float m_cameraMoveSpeed = 8.0f;
		float m_cameraRotationSpeed = 64.0f;

		glm::vec3 m_camerPos = {0.0f,0.0f,0.0f};
		float m_cameraRotation = 0.0f;


		bool onMouseScroll(MouseScrollEvent& e);
		bool onResizeEvent(WindowResizeEvent& e);
	};
}