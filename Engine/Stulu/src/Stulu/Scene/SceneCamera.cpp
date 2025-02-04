#include "st_pch.h"
#include "SceneCamera.h"
#include "Stulu/Core/Input.h"
#include "Stulu/Core/Keycodes.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Core/MouseButtonCodes.h"
#include "Stulu/Renderer/RenderCommand.h"

namespace Stulu {
	SceneCamera::SceneCamera(float aspectRatio, float fov, float zNear, float zFar, uint32_t samples)
		:m_aspectRatio(aspectRatio), m_fov(fov), m_zNear(zNear), m_zFar(zFar) {

		m_cam = createRef<PerspectiveCamera>(m_fov, m_aspectRatio, m_zNear, m_zFar, FrameBufferSpecs(1, 1, samples));
	}
	void SceneCamera::onUpdate(Timestep timestep) {
		m_mouseDelta = Input::getMouseDelta() * 0.003f;
		m_mouseDelta = glm::clamp(m_mouseDelta, glm::vec2(-1.0f), glm::vec2(1.0f));
		m_frustum = VFC::createFrustum(m_aspectRatio, m_zNear, m_zFar, m_fov, m_transform);
	}
	void SceneCamera::updateMove(Timestep timestep) {
		mouseTranslateMove(timestep);
		mouseLookMove();
	}
	void SceneCamera::mouseTranslateMove(Timestep timestep) {
		if (Input::isMouseDown(MOUSE_BUTTON_MIDDLE)) {
			m_transform.position += m_transform.right * -m_mouseDelta.x * m_cameraMoveSpeed * (float)timestep.getMilliseconds();
			m_transform.position += m_transform.up * m_mouseDelta.y * m_cameraMoveSpeed * (float)timestep.getMilliseconds();
		}
	}
	void SceneCamera::mouseLookMove() {
		if (Input::isMouseDown(MOUSE_BUTTON_2)) {
			float yawSign = m_transform.up.y < 0 ? -1.0f : 1.0f;
			m_yaw += yawSign * m_mouseDelta.x * m_cameraSensitivity;
			m_pitch += m_mouseDelta.y * m_cameraSensitivity;
			m_transform.rotation = glm::vec3(-m_pitch, -m_yaw, .0f);
		}
	}
	void SceneCamera::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(ST_BIND_EVENT_FN(SceneCamera::onMouseScrolledEvent));
	}
	void SceneCamera::onResize(float width, float height) {
		m_aspectRatio = width / height;
		m_cam->setProjection(m_fov, m_aspectRatio, m_zNear, m_zFar);
		m_cam->getFrameBuffer()->resize((uint32_t)width, (uint32_t)height);
	}
	bool SceneCamera::onMouseScrolledEvent(MouseScrollEvent& e) {
		m_transform.position += m_transform.forward * e.getYOff();
		return false;
	}
}