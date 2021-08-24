#include "st_pch.h"
#include "SceneCamera.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"
#include "Stulu/Renderer/RenderCommand.h"

namespace Stulu {
	SceneCamera::SceneCamera(float aspectRatio, float fov, float zNear, float zFar)
		:m_aspectRatio(aspectRatio), m_fov(fov), m_zNear(zNear), m_zFar(zFar), m_cam(m_fov, m_aspectRatio, m_zNear, m_zFar) {

	}
	void SceneCamera::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		mouseTranslateMove(timestep);
		mouseLookMove();
		m_cam.recalculateViewMatrix();
		
	}
	void SceneCamera::mouseTranslateMove(Timestep timestep) {
		if (Input::isMouseDown(MOUSE_BUTTON_1)) {
			m_transform.position += m_cam.getRightDirection() * -m_mouseDelta.x * m_cameraMoveSpeed *(float)timestep.getMilliseconds();
			m_transform.position += m_cam.getUpDirection() * m_mouseDelta.y * m_cameraMoveSpeed *(float)timestep.getMilliseconds();


			m_cam.setTransform(m_transform);
		}
	}
	void SceneCamera::mouseLookMove() {
		if (Input::isMouseDown(MOUSE_BUTTON_2)) {
			float yawSign = m_cam.getUpDirection().y < 0 ? -1.0f : 1.0f;
			m_yaw += yawSign * m_mouseDelta.x * m_cameraSensitivity;
			m_pitch += m_mouseDelta.y * m_cameraSensitivity;
			m_transform.setRotation(-m_pitch, -m_yaw, .0f);
		}

		m_cam.setTransform(m_transform);


		m_mouseDelta = glm::vec2(Input::getMouseX() - m_lastMouseXPos, Input::getMouseY() - m_lastMouseYPos) * 0.003f;
		m_lastMouseXPos = Input::getMouseX();
		m_lastMouseYPos = Input::getMouseY();
	}
	bool SceneCamera::onMouseScrolledEvent(MouseScrollEvent& e) {
		ST_PROFILING_FUNCTION();
		m_transform.position += m_cam.getForwardDirection() * e.getYOff();
		m_cam.setTransform(m_transform);
		m_cam.recalculateViewMatrix();
		return false;
	}
	bool SceneCamera::onResizeEvent(WindowResizeEvent& e) {
		ST_PROFILING_FUNCTION();
		m_aspectRatio = (float)e.getWidth() / (float)e.getHeight();
		m_cam.setProjection(m_fov, m_aspectRatio, m_zNear, m_zFar);
		return false;
	}
	bool SceneCamera::onKeyPressed(KeyDownEvent& e) {
		ST_PROFILING_FUNCTION();
		if (e.getKeyCode() == wireFrameSwitchKey) {
			wireFrame = !wireFrame;
			RenderCommand::setWireFrame(wireFrame);
		}
		return false;
	}
	void SceneCamera::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(ST_BIND_EVENT_FN(SceneCamera::onMouseScrolledEvent));
		dispatcher.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(SceneCamera::onResizeEvent));
		dispatcher.dispatch<KeyDownEvent>(ST_BIND_EVENT_FN(SceneCamera::onKeyPressed));
	}
}