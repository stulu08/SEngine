#pragma once
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Scene/Components.h"
#include "Stulu/Events/MouseEvent.h"
#include "Stulu/Events/KeyEvent.h"

namespace Stulu {
	class STULU_API SceneCamera {
	public:
		SceneCamera(float aspectRatio, float fov, float zNear, float zFar);

		void onUpdate(Timestep timestep);
		void updateMove(Timestep timestep);
		void onEvent(Event& e);
		void onResize(float width, float height);

		Ref<Camera>& getCamera() { return m_cam; }
		const Ref<Camera>& getCamera() const { return m_cam; }


		float getAspectRatio() { return m_aspectRatio; }


		const TransformComponent& getTransform() const { return m_transform; }
		TransformComponent& getTransform() { return m_transform; }
		void setTransform(const TransformComponent& transform) { m_transform = transform; }

	private:
		glm::vec2 m_mouseDelta = glm::vec2(0.0f);
		float m_aspectRatio, m_fov, m_zNear, m_zFar, m_lastMouseXPos = 0, m_lastMouseYPos = 0;
		float m_pitch = 0, m_yaw = 0;
		Ref<Camera> m_cam;
		TransformComponent m_transform;

		float m_cameraMoveSpeed = 1.0f;
		float m_cameraSensitivity = .8f;


		void mouseTranslateMove(Timestep timestep);
		void mouseLookMove();

		bool onMouseScrolledEvent(MouseScrollEvent& e);
		bool onMouseMoveEvent(MouseMoveEvent& e);
		bool onResizeEvent(WindowResizeEvent& e);

	};
}