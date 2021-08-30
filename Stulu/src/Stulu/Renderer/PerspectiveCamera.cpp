#include "st_pch.h"
#include "PerspectiveCamera.h"
#include "Stulu/Math/Math.h"
namespace Stulu {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float z_near, float z_far)
		:m_projcetionMatrix(glm::perspective(glm::radians(fov), aspect, z_near, z_far)), m_viewMatrix(1.0f) {
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	const void PerspectiveCamera::setProjection(float fov, float aspect, float z_near, float z_far)
	{
		ST_PROFILING_FUNCTION();
		m_projcetionMatrix = glm::perspective(glm::radians(fov), aspect, z_near, z_far);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	void PerspectiveCamera::recalculateViewMatrix()
	{
		ST_PROFILING_FUNCTION();
		glm::quat orientation = glm::quat(glm::radians(m_rotation));
		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		m_viewMatrix = glm::inverse(m_viewMatrix);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}
	const glm::vec3 PerspectiveCamera::getUpDirection() {
		return glm::rotate(glm::quat(glm::radians(m_rotation)), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	const glm::vec3 PerspectiveCamera::getRightDirection() {
		return glm::rotate(glm::quat(glm::radians(m_rotation)), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	const glm::vec3 PerspectiveCamera::getForwardDirection() {
		return glm::rotate(glm::quat(glm::radians(m_rotation)), glm::vec3(0.0f, 0.0f, -1.0f));
	}
}
