#include "st_pch.h"
#include "PerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Stulu {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float z_near, float z_far)
		:m_projcetionMatrix(glm::perspective(glm::radians(fov), aspect, z_near, z_far)), m_viewMatrix(1.0f) {
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
		m_zFar = z_far;
		m_zNear = z_near;
	}

	void PerspectiveCamera::setProjection(float fov, float aspect, float z_near, float z_far)
	{
		ST_PROFILING_FUNCTION();
		m_projcetionMatrix = glm::perspective(glm::radians(fov), aspect, z_near, z_far);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
		m_zFar = z_far;
		m_zNear = z_near;
	}

	void PerspectiveCamera::recalculateViewMatrix()
	{
		ST_PROFILING_FUNCTION();
		glm::quat orientation = m_transform.getOrientation();
		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_transform.position) * glm::toMat4(orientation);
		m_viewMatrix = glm::inverse(m_viewMatrix);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}
	glm::vec3 PerspectiveCamera::getUpDirection() {
		return glm::rotate(m_transform.getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 PerspectiveCamera::getRightDirection() {
		return glm::rotate(m_transform.getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 PerspectiveCamera::getForwardDirection() {
		return glm::rotate(m_transform.getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}
}
