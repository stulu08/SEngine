#pragma once
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Events/KeyEvent.h"
#include "Stulu/Scene/Components.h"

namespace Stulu {
	class SceneCamera {
	public:
		SceneCamera(float aspectRatio, float fov, float zNear, float zFar);

		void onUpdate(Timestep timestep);
		void onEvent(Event& e);
		void onResize(float width, float height);

		PerspectiveCamera& getCamera() { return m_cam; }
		const PerspectiveCamera& getCamera() const { return m_cam; }


		float getAspectRatio() { return m_aspectRatio; }

		uint32_t wireFrameSwitchKey = 70;

		Transform& getTransform() { return m_transform; }
		void setTransform(const Transform& transform) { m_transform = transform; }

	private:
		glm::vec2 m_mouseDelta = glm::vec2(0.0f);
		float m_aspectRatio, m_fov, m_zNear, m_zFar, m_lastMouseXPos = 0, m_lastMouseYPos = 0;
		float m_pitch = 0, m_yaw = 0;
		bool wireFrame = false;
		PerspectiveCamera m_cam;

		Transform m_transform;

		float m_cameraMoveSpeed = 1.0f;
		float m_cameraSensitivity = .8f;


		void mouseTranslateMove(Timestep timestep);
		void mouseLookMove();

		bool onMouseScrolledEvent(MouseScrollEvent& e);
		bool onResizeEvent(WindowResizeEvent& e);
		bool onKeyPressed(KeyDownEvent& e);

	};
}