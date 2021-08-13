#include "st_pch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Stulu{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		:m_projcetionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 100.0f)), m_viewMatrix(1.0f) {

		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::setProjection(float left, float right, float bottom, float top)
	{
		ST_PROFILING_FUNCTION();
		m_projcetionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 100.0f);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::recalculateViewMatrix()
	{
		ST_PROFILING_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

		m_viewMatrix = glm::inverse(transform);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}
}