#include "st_pch.h"
#include "OrthographicCamera.h"
#include "Stulu/Math/Math.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Stulu{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar)
		:m_projcetionMatrix(glm::ortho(left, right, bottom, top, zNear, zFar)), m_viewMatrix(1.0f) {

		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	const void OrthographicCamera::setProjection(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		ST_PROFILING_FUNCTION();
		m_projcetionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}

	void OrthographicCamera::recalculateViewMatrix()
	{
		ST_PROFILING_FUNCTION();
		glm::mat4 transform = Math::createMat4(m_position, m_rotation, glm::vec3(1.0f));

		m_viewMatrix = glm::inverse(transform);
		m_viewProjcetionMatrix = m_projcetionMatrix * m_viewMatrix;
	}
	const glm::vec3 OrthographicCamera::getUpDirection() {
		return glm::rotate(glm::quat(glm::radians(m_rotation)), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	const glm::vec3 OrthographicCamera::getRightDirection() {
		return glm::rotate(glm::quat(glm::radians(m_rotation)), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	const glm::vec3 OrthographicCamera::getForwardDirection() {
		return glm::rotate(glm::quat(glm::radians(m_rotation)), glm::vec3(0.0f, 0.0f, -1.0f));
	}
}