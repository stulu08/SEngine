#include "st_pch.h"
#include "SceneCamera.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/MouseButtonCodes.h"
#include "Stulu/Renderer/RenderCommand.h"

namespace Stulu {
	SceneCamera::SceneCamera(float aspectRatio, float fov, float zNear, float zFar)
		:m_aspectRatio(aspectRatio), m_fov(fov), m_zNear(zNear), m_zFar(zFar) {

		ST_PROFILING_FUNCTION();
		m_cam = createRef<PerspectiveCamera>(m_fov, m_aspectRatio, m_zNear, m_zFar);


	}
	void SceneCamera::onUpdate(Timestep timestep) {
		ST_PROFILING_FUNCTION();

		m_mouseDelta = glm::vec2(Input::getMouseX() - m_lastMouseXPos, Input::getMouseY() - m_lastMouseYPos) * 0.003f;
		m_lastMouseXPos = Input::getMouseX();
		m_lastMouseYPos = Input::getMouseY();

		m_mouseDelta = glm::clamp(m_mouseDelta, glm::vec2(-1.0f), glm::vec2(1.0f));

	}
	void SceneCamera::updateMove(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		mouseTranslateMove(timestep);
		mouseLookMove();
	}
	void SceneCamera::mouseTranslateMove(Timestep timestep) {
		ST_PROFILING_FUNCTION();
		if (Input::isMouseDown(MOUSE_BUTTON_1)) {
			m_transform.position += m_transform.rightDirection() * -m_mouseDelta.x * m_cameraMoveSpeed *(float)timestep.getMilliseconds();
			m_transform.position += m_transform.upDirection() * m_mouseDelta.y * m_cameraMoveSpeed *(float)timestep.getMilliseconds();
		}
	}
	void SceneCamera::mouseLookMove() {
		if (Input::isMouseDown(MOUSE_BUTTON_2)) {
			float yawSign = m_transform.upDirection().y < 0 ? -1.0f : 1.0f;
			m_yaw += yawSign * m_mouseDelta.x * m_cameraSensitivity;
			m_pitch += m_mouseDelta.y * m_cameraSensitivity;
			m_transform.rotation = glm::vec3(-m_pitch, -m_yaw, .0f);
		}
	}
	void SceneCamera::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(ST_BIND_EVENT_FN(SceneCamera::onMouseScrolledEvent));
		dispatcher.dispatch<MouseMoveEvent>(ST_BIND_EVENT_FN(SceneCamera::onMouseMoveEvent));
		dispatcher.dispatch<WindowResizeEvent>(ST_BIND_EVENT_FN(SceneCamera::onResizeEvent));
	}
	void SceneCamera::onResize(float width, float height) {
		ST_PROFILING_FUNCTION();
		m_aspectRatio = width / height;
		m_cam->setProjection(m_fov, m_aspectRatio, m_zNear, m_zFar);
		m_cam->getFrameBuffer()->resize((uint32_t)width, (uint32_t)height);
	}
	bool SceneCamera::onMouseScrolledEvent(MouseScrollEvent& e) {
		ST_PROFILING_FUNCTION();
		m_transform.position += m_transform.forwardDirection() * e.getYOff();
		return false;
	}
	bool SceneCamera::onMouseMoveEvent(MouseMoveEvent& e) {
		ST_PROFILING_FUNCTION();
		return false;
	}
	bool SceneCamera::onResizeEvent(WindowResizeEvent& e) {
		ST_PROFILING_FUNCTION();
		onResize((float)e.getWidth(), (float)e.getHeight());
		return false;
	}
}