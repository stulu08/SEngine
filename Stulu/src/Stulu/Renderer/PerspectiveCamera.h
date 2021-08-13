#pragma once
#pragma once
#include <glm/glm.hpp>
#include "Stulu/Renderer/Camera.h"
#include <Stulu/Renderer/Transform.h>

namespace Stulu {
	class PerspectiveCamera : public Camera{
	public:
		PerspectiveCamera(float fov, float aspect, float z_near, float z_far);

		void setProjection(float fov, float aspect, float z_near, float z_far);

		const glm::mat4& getProjectionMatrix() const override { return m_projcetionMatrix; }
		const glm::mat4& getViewMatrix() const override { return m_viewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const override { return m_viewProjcetionMatrix; }
		const glm::vec3& getPos() override { return m_transform.getPos(); }

		Transform& getTransform() { return m_transform; recalculateViewMatrix();}
		void setTransform(const Transform& transform) { m_transform = transform; recalculateViewMatrix(); }

		const float& getZFar() { return m_zFar; }
		const float& getZNear() { return m_zNear; }

		void recalculateViewMatrix();
		glm::vec3 getUpDirection();
		glm::vec3 getRightDirection();
		glm::vec3 getForwardDirection();
	private:
		float m_zNear, m_zFar;

		glm::mat4 m_projcetionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjcetionMatrix;

		Transform m_transform;
	};
}


