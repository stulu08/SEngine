#pragma once
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/MouseEvent.h"
namespace Stulu {
	class PerspectiveCameraController {
	public:
		PerspectiveCameraController(float aspectRatio, float fov, float zNear, float zFar);

		void onUpdate(Timestep timestep);
		void onEvent(Event& e);

		PerspectiveCamera& getCamera() { return m_cam; }
		const PerspectiveCamera& getCamera() const { return m_cam; }

		float cameraBaseMoveSpeed = 8.0f;
		float cameraSensitivity = .8f;

		Transform& getTransform() { return m_transform; }
		void setTransform(const Transform& transform) { m_transform = transform; }

	private:
		float m_aspectRatio, m_fov, m_zNear, m_zFar;
		float m_pitch = 0, m_yaw = 0;
		PerspectiveCamera m_cam;

		Transform m_transform;

		float m_cameraMoveSpeed = 8.0f;

		bool onResizeEvent(WindowResizeEvent& e);

		glm::vec3 getUpDirection();
		glm::vec3 getRightDirection();
		glm::vec3 getForwardDirection();
	};
}