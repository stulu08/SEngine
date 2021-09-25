#include "st_pch.h"
#include "PerspectiveCamera.h"
#include "Stulu/Math/Math.h"
namespace Stulu {
	PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float z_near, float z_far)
		:m_projcetionMatrix(glm::perspective(glm::radians(fov), aspect, z_near, z_far)) {
	}

	const void PerspectiveCamera::setProjection(float fov, float aspect, float z_near, float z_far) {
		ST_PROFILING_FUNCTION();
		m_projcetionMatrix = glm::perspective(glm::radians(fov), aspect, z_near, z_far);
	}
}
