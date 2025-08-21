#include "st_pch.h"
#include "SceneCamera.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Input/Input.h"
#include "Stulu/Input/Keycodes.h"
#include "Stulu/Input/MouseButtonCodes.h"
#include "Stulu/Renderer/RenderCommand.h"

namespace Stulu {
	SceneCamera::SceneCamera(float aspectRatio, float fov, float zNear, float zFar, MSAASamples samples)
		: m_aspectRatio(aspectRatio), m_fov(fov), 
		m_zNear(zNear), m_zFar(zFar), 
		m_cam(CameraMode::Perspective, FrameBufferSpecs(1, 1, samples), true) {

		m_cam.SetPrespective(m_fov, m_aspectRatio, m_zNear, m_zFar);
		m_transform.SetUpdatePhysics(false);
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
			glm::vec3 position = m_transform.position;
			position += m_transform.GetRight() * -m_mouseDelta.x * m_cameraMoveSpeed * (float)timestep.getMilliseconds();
			position += m_transform.GetUp() * m_mouseDelta.y * m_cameraMoveSpeed * (float)timestep.getMilliseconds();
			m_transform.SetWorldPosition(position);
		}
	}
	void SceneCamera::mouseLookMove() {
		if (Input::isMouseDown(MOUSE_BUTTON_2)) {
			float pitchDelta = -m_mouseDelta.y * m_cameraSensitivity * 5.0f;
			float yawDelta = -m_mouseDelta.x * m_cameraSensitivity * 5.0f;

			glm::quat pitchQuat = glm::angleAxis(glm::radians(pitchDelta), m_transform.GetRight());
			glm::quat yawQuat = glm::angleAxis(glm::radians(yawDelta), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::quat newRotation = yawQuat * pitchQuat * m_transform.GetWorldRotation();
			m_transform.SetWorldRotation(glm::normalize(newRotation));
		}
	}
	void SceneCamera::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrollEvent>(ST_BIND_EVENT_FN(SceneCamera::onMouseScrolledEvent));
	}
	void SceneCamera::onResize(float width, float height) {
		m_aspectRatio = width / height;
		m_cam.SetPrespective(m_fov, m_aspectRatio, m_zNear, m_zFar);
		m_cam.ResizeFrameBuffer((uint32_t)width, (uint32_t)height);
	}
	bool SceneCamera::onMouseScrolledEvent(MouseScrollEvent& e) {
		m_transform.SetWorldPosition(m_transform.position + m_transform.GetForward() * e.getYOff());
		return false;
	}
}