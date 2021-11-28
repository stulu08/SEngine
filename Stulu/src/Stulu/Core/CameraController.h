#pragma once
#include "Stulu/Renderer/OrthographicCamera.h"
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Events/KeyEvent.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Math/Math.h"
#include <Stulu/Scene/Components.h>

namespace Stulu {
	struct CamerControllerSetting {
		float fov = 80.0f, zoom = 1.0f, zNear = .01f, zFar = 250.0f;
		float cameraBaseMoveSpeed = 8.0f, cameraSensitivity = .8f, cameraRotationSpeed = 64.0f;
		bool wireFrame = false, m_controlls = true;
		uint32_t wireFrameSwitchKey = 70;

	};
	class STULU_API CameraController {
	public:
		CameraController(CamerControllerSetting* settings, CameraMode mode = CameraMode::Perspective);

		void onUpdate(Timestep timestep);
		void onEvent(Event& e);
		void onResize(float width, float height);

		inline Ref<Camera>& getCamera() {
			return m_activeCamera;
		}

		TransformComponent& getTransform() { return m_transform; }
		void setTransform(const TransformComponent& transform) { m_transform = transform; }

		float getAspectRatio() { return m_aspectRatio; }

		CameraMode& getMode() { return m_mode; }
		inline void setMode(const CameraMode& mode) {
			if (mode == CameraMode::Orthographic)
				m_activeCamera = createRef<OrthographicCamera>(-m_aspectRatio * m_settings->zoom, m_aspectRatio * m_settings->zoom, -m_settings->zoom, m_settings->zoom, m_settings->zNear, m_settings->zFar);
			else
				m_activeCamera = createRef<PerspectiveCamera>(m_settings->fov, m_aspectRatio, m_settings->zNear, m_settings->zFar);
			m_mode = mode; 
		}
	private:
		float lastMouseXPos = 0;
		float lastMouseYPos = 0;
		float m_speed = 12.0f;
		float m_pitch = .0f, m_yaw = .0f;
		float m_aspectRatio = (float)Application::get().getWindow().getWidth() / (float)Application::get().getWindow().getHeight();

		glm::vec2 mouseDelta = glm::vec2(0.0f);
		CameraMode m_mode = CameraMode::Perspective;
		CamerControllerSetting* m_settings;
		TransformComponent m_transform;

		Stulu::Ref<Camera> m_activeCamera;

		bool onResizeEvent(WindowResizeEvent& e);
		bool onKeyPressed(KeyDownEvent& e);
	};
}