#pragma once
#include "Stulu/Renderer/PerspectiveCamera.h"
#include "Stulu/Core/Timestep.h"
#include "Stulu/Events/ApplicationEvent.h"
#include "Stulu/Events/MouseEvent.h"
#include <Stulu/Events/KeyEvent.h>
namespace Stulu {
	class PerspectiveCameraController {
	public:
		PerspectiveCameraController(float aspectRatio, float fov, float zNear, float zFar, bool move = true, bool look = true);

		void onUpdate(Timestep timestep);
		void onEvent(Event& e);

		PerspectiveCamera& getCamera() { return m_cam; }
		const PerspectiveCamera& getCamera() const { return m_cam; }

		float cameraBaseMoveSpeed = 8.0f;
		float cameraSensitivity = .8f;

		float getAspectRatio() { return m_aspectRatio; }

		uint32_t wireFrameSwitchKey = 70;

		Transform& getTransform() { return m_transform; }
		void setTransform(const Transform& transform) { m_transform = transform; }

	private:
		float m_aspectRatio, m_fov, m_zNear, m_zFar;
		float m_pitch = 0, m_yaw = 0;
		bool wireFrame = false, m_move, m_look;
		PerspectiveCamera m_cam;

		Transform m_transform;

		float m_cameraMoveSpeed = 8.0f;

		bool onResizeEvent(WindowResizeEvent& e);
		bool onKeyPressed(KeyDownEvent& e);


	};
}