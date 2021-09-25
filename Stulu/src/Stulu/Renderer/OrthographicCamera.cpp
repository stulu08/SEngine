#include "st_pch.h"
#include "OrthographicCamera.h"
#include "Stulu/Math/Math.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Stulu{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar)
		:m_projcetionMatrix(glm::ortho(left, right, bottom, top, zNear, zFar)) {

	}

	const void OrthographicCamera::setProjection(float left, float right, float bottom, float top, float zNear, float zFar){
		ST_PROFILING_FUNCTION();
		m_projcetionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
	}
}