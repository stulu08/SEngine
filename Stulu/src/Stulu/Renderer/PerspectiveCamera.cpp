#include "st_pch.h"
#include "PerspectiveCamera.h"
#include <glm/gtc/matrix_transform.hpp>
namespace Stulu {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float z_near, float z_far)
		:m_projcetionMatrix(glm::perspective(glm::radians(fov), aspect, z_near, z_far)), m_viewMatrix(1.0f) {

		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	void PerspectiveCamera::setProjection(float fov, float aspect, float z_near, float z_far)
	{
		m_projcetionMatrix = glm::perspective(glm::radians(fov), aspect, z_near, z_far);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	void PerspectiveCamera::recalculateViewMatrix()
	{
		glm::mat4 transform = m_transform;

		m_viewMatrix = glm::inverse(transform);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}
}
