#pragma once
#include <glm/glm.hpp>
#include "Stulu/Renderer/Camera.h"

namespace Stulu {
	class PerspectiveCamera : public Camera{
	public:
		PerspectiveCamera(float fov, float aspect, float z_near, float z_far);
		const virtual void setProjection(float fov, float aspect, float z_near, float z_far) override;
		const glm::mat4& getProjectionMatrix() const override { return m_projcetionMatrix; }
	private:
		glm::mat4 m_projcetionMatrix;
	};
}


