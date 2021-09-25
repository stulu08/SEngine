#pragma once
#include <glm/glm.hpp>
#include "Stulu/Renderer/Camera.h"

namespace Stulu {
	class OrthographicCamera : public Camera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar);
		const virtual void setProjection(float left, float right, float bottom, float top, float zNear, float zFar) override;
		const glm::mat4& getProjectionMatrix() const override { return m_projcetionMatrix; }
	private:
		glm::mat4 m_projcetionMatrix;
	};
}